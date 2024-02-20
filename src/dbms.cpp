#include <dbms.h>

#include <unordered_set>
#include <stdio.h>
#include <string.h>

namespace dbms {

void initSubmodules(core::LogLevel logLevel) {
    core::initLoggingSystem(logLevel, nullptr, 0);
    core::rndInit();

    core::setGlobalAssertHandler([](const char* failedExpr, const char* file, i32 line, const char* funcName, const char* errMsg) {
        printf(ANSI_RED_START() ANSI_BOLD_START()
                "[ASSERTION]:\n  [EXPR]: %s\n  [FUNC]: %s\n  [FILE]: %s:%d\n  [MSG]: %s"
               ANSI_RESET() "\n",
            failedExpr, funcName, file, line, errMsg);
        throw std::runtime_error("Assertion failed!");
    });

    logInfo("DBMS_DEBUG = " DBMS_DEBUG);
    logInfo("DBMS_VERSION = " DBMS_VERSION);
    logInfo("DBMS_BINARY_PATH = " DBMS_BINARY_PATH);
    logInfo("DBMS_DATA_PATH = " DBMS_DATA_PATH);
}

bool loadDatabase(const char* path, Database& db) {
    TRACE_BLOCK_CPU_TIME("Loading database took");

    namespace fs = std::filesystem;
    constexpr const char* COLUMN_PREFIX = Database::DBMS_DATA_COLUMN_PREFIX.data();

    db.path = path;
    db.name = fs::path(path).filename().string();
    db.tables.clear();

    for (const auto & columnDir : fs::directory_iterator(db.path)) {
        if (columnDir.is_directory()) {
            Database::Table t;
            t.name = columnDir.path().stem().string();
            t.path = columnDir.path();

            for (const auto & columnFile : fs::directory_iterator(columnDir.path())) {
                if (columnFile.is_regular_file()) {
                    std::string colName = columnFile.path().stem().string();
                    if (colName != "value" && colName.find(COLUMN_PREFIX) != std::string::npos) {
                        colName = colName.erase(0, colName.find_last_of(COLUMN_PREFIX) + 1);
                        t.names.colNames.push_back(std::move(colName));
                    }
                    else {
                        t.names.valueColNames.push_back(colName);
                    }
                }
            }

            if (t.names.colNames.empty()) {
                logFatal("Table %s has no columns", t.name.c_str());
                return false;
            }

            std::sort(t.names.colNames.begin(), t.names.colNames.end());

            auto createColumn = [](const auto& dir,
                                   const std::string& colName,
                                   Column& c,
                                   MemoryMappedSrc& mms) -> bool {

                constexpr const char* DATA_FILE_EXTENSION = Database::DBMS_DATA_FILE_EXTENSION.data();

                std::string pathToFile;
                if (colName == "value") {
                    pathToFile = (dir.path() / (colName + DATA_FILE_EXTENSION)).string();
                }
                else {
                    pathToFile = (dir.path() / (COLUMN_PREFIX + colName + DATA_FILE_EXTENSION)).string();
                }

                bool ok = openMemoryMappedFile(pathToFile, mms);
                if (!ok) {
                    logFatal("Failed to create a memory mapped file for '%s'", pathToFile.c_str());
                    return false;
                }

                const u64* dataptr = reinterpret_cast<const u64*>(mms.data());
                u64 fileSizeInU64 = mms.size() / sizeof(u64);
                c = Column(fileSizeInU64, dataptr);
                return true;
            };

            for (const auto& colName : t.names.colNames) {
                Column c;
                MemoryMappedSrc mms;
                if (!createColumn(columnDir, colName, c, mms)) {
                    logFatal("Failed to create column '%s'", colName.c_str());
                    return false;
                }

                t.columns.push_back(std::move(c));
                t.mappedFiles.push_back(std::move(mms));
            }

            for (const auto& colName : t.names.valueColNames) {
                Column c;
                MemoryMappedSrc mms;
                if (!createColumn(columnDir, colName, c, mms)) {
                    logFatal("Failed to create column '%s'", colName.c_str());
                    return false;
                }

                t.columns.push_back(std::move(c));
                t.mappedFiles.push_back(std::move(mms));
            }


            db.tables.push_back(std::move(t));
        }
    }

    return true;
}

void optimizeAlignmentExecutionOrder(Database& db) {
    if (db.tables.empty()) return;

    std::vector<dbms::Database::Table> optimizedTableOrder;
    optimizedTableOrder.reserve(db.tables.size());

    std::unordered_set<addr_size> joinWithBothClustered;
    std::unordered_set<addr_size> joinWithOneClustered;
    std::unordered_set<addr_size> joinWithNoClustered;

    // Group together all tables that have the same clustered index
    {
        for (addr_size i = 0; i < db.tables.size(); i++) {
            if (db.tables[i].names.colNames.empty()) continue;

            const std::string& a = db.tables[i].names.colNames[0];

            addr_off idx = -1;
            for (addr_size j = 0; j < db.tables.size(); j++) {
                if (i == j) continue;
                if (db.tables[j].names.colNames.empty()) continue;

                const std::string& b = db.tables[j].names.colNames[0];
                if (a == b) {
                    idx = j;
                    break;
                }
            }

            if (idx >= 0) {
                joinWithBothClustered.insert(i);
            }
        }
    }

    // Group together all tables that can be joined on one cluster index
    {
        for (addr_size i = 0; i < db.tables.size(); i++) {
            if (db.tables[i].names.colNames.empty()) continue;
            if (joinWithBothClustered.count(i) > 0) continue;

            const std::string& a = db.tables[i].names.colNames[0];

            addr_off idx = -1;
            for (addr_size j = 0; j < db.tables.size(); j++) {
                if (i == j) continue;
                if (db.tables[j].names.colNames.empty()) continue;

                for (const auto& col : db.tables[j].names.colNames) {
                    if (a == col) {
                        idx = j;
                        break;
                    }
                }

                if (idx >= 0) break;
            }

            if (idx >= 0) {
                joinWithOneClustered.insert(i);
            }
        }
    }

    // Group together with least priority tables that can be joined only on un-clustered indices:
    {
        for (addr_size i = 0; i < db.tables.size(); i++) {
            if (db.tables[i].names.colNames.empty()) continue;

            if (joinWithBothClustered.count(i) == 0 && joinWithOneClustered.count(i) == 0) {
                joinWithNoClustered.insert(i);
            }
        }
    }

    // Add the tables to the result in the order of their priority:

    for (addr_size i : joinWithBothClustered) {
        optimizedTableOrder.push_back(std::move(db.tables[i]));
    }

    for (addr_size i : joinWithOneClustered) {
        optimizedTableOrder.push_back(std::move(db.tables[i]));
    }

    for (addr_size i : joinWithNoClustered) {
        optimizedTableOrder.push_back(std::move(db.tables[i]));
    }

    db.tables = std::move(optimizedTableOrder); // Move the result back to the original vector
}

IndexTranslationTable createIndexTranslationTable(const ColumnNames& from, const ColumnNames& to) {
    IndexTranslationTable ttable;
    for (u64 i = 0; i < from.colNames.size(); i++) {
        for (u64 j = 0; j < to.colNames.size(); j++) {
            if (from.colNames[i] == to.colNames[j]) {
                ttable.push_back({ i, j });
                break;
            }
        }
    }
    return ttable;
}

WriteOrder createTableWriteOrder(const ColumnNames& a, const ColumnNames& b, const ColumnNames& unique) {
    WriteOrder writeOrder;
    std::vector<bool> used (unique.colNames.size(), false);

    for (u64 i = 0; i < a.colNames.size(); i++) {
        const auto& leftCol = a.colNames[i];
        for (u64 j = 0; j < unique.colNames.size(); j++) {
            if (unique.colNames[j] == leftCol) {
                writeOrder.first.push_back({ j, i });
                used[j] = true;
                break;
            }
        }
    }

    for (u64 i = 0; i < b.colNames.size(); i++) {
        const auto& rightCol = b.colNames[i];
        for (u64 j = 0; j < unique.colNames.size(); j++) {
            if (!used[j] && unique.colNames[j] == rightCol) {
                writeOrder.second.push_back({ j, i });
                used[j] = true;
                break;
            }
        }
    }

    return writeOrder;
}

JoinResult JoinResult::createFromNames(const ColumnNames& a, const ColumnNames& b) {
    JoinResult result;

    for (u64 i = 0; i < a.colNames.size(); i++) {
        core::push_back_unique(result.names.colNames, a.colNames[i]);
    }

    for (u64 i = 0; i < b.colNames.size(); i++) {
        core::push_back_unique(result.names.colNames, b.colNames[i]);
    }

    std::sort(result.names.colNames.begin(), result.names.colNames.end());

    for (u64 i = 0; i < a.valueColNames.size(); i++) {
        result.names.valueColNames.emplace_back("Column" + std::to_string(result.names.valueColNames.size() + 1));
    }

    for (u64 i = 0; i < b.valueColNames.size(); i++) {
        result.names.valueColNames.emplace_back("Column" + std::to_string(result.names.valueColNames.size() + 1));
    }

    result.columns.resize(result.names.colNames.size() + result.names.valueColNames.size());
    return result;
}

void appendRowToResult(JoinResult& result,
                       const ColumnNames& leftColNames,
                       const ColumnNames& rightColNames,
                       const ColumnGroup& left,
                       const ColumnGroup& right,
                       u64 leftRowIdx, u64 rightRowIdx) {

    u64 startOfValuesInLeft = leftColNames.colNames.size();
    u64 startOfValuesInRight = rightColNames.colNames.size();
    u64 startOfValuesInResult = result.names.colNames.size();

    const auto& writeOrder = createTableWriteOrder(leftColNames, rightColNames, result.names);

    // Append the matching rows to the result.
    for (auto& [resultIdx, leftIdx] : writeOrder.first) {
        auto lds = left[leftIdx].data();
        result.columns[resultIdx].append(lds[leftRowIdx]);
    }
    for (auto& [resultIdx, rightIdx] : writeOrder.second) {
        auto rds = right[rightIdx].data();
        result.columns[resultIdx].append(rds[rightRowIdx]);
    }

    u64 valueWriteIdx = startOfValuesInResult;
    for (u64 i = startOfValuesInLeft; i < left.size(); i++) {
        auto lds = left[i].data();
        result.columns[valueWriteIdx].append(lds[leftRowIdx]);
        valueWriteIdx++;
    }
    for (u64 i = startOfValuesInRight; i < right.size(); i++) {
        auto rds = right[i].data();
        result.columns[valueWriteIdx].append(rds[rightRowIdx]);
        valueWriteIdx++;
    }
}

bool checkSecondaryKeys(const ColumnNames& leftColNames,
                        const ColumnNames& rightColNames,
                        const ColumnGroup& left,
                        const ColumnGroup& right,
                        u64 leftRowIdx, u64 rightRowIdx) {

    bool match = true;
    const auto& leftToRightTranslationTable = createIndexTranslationTable(leftColNames, rightColNames);

    // Check if all of the secondary indices match.
    for (u64 i = 1; i < leftToRightTranslationTable.size(); i++) {
        const auto& [leftColIdx, rightColIdx] = leftToRightTranslationTable[i];
        auto ldataCurr = left[leftColIdx].data();
        auto rdataCurr = right[rightColIdx].data();
        if (ldataCurr[leftRowIdx] != rdataCurr[rightRowIdx]) {
            match = false;
            break;
        }
    }

    return match;
}

u64 sumSquared(JoinResult& cols) {
    TRACE_BLOCK_CPU_TIME("sumSquared");

    if (cols.columns.empty()) return 0;

    u64 valuesStart = cols.names.colNames.size();
    u64 total = 0;
    std::vector<u64> sumSquaredColumn (cols.columns[0].data().size());

    for (u64 i = 0; i < sumSquaredColumn.size(); i++) {
        u64 rowSum = 0;
        for (u64 j = valuesStart; j < cols.columns.size(); j++) {
            auto cdata = cols.columns[j].data();
            rowSum += cdata[i];
        }
        sumSquaredColumn[i] = rowSum * rowSum;
        total += sumSquaredColumn[i];
    }

    cols.columns.emplace_back(Column { std::move(sumSquaredColumn) });
    cols.names.valueColNames.push_back("Sum^2");
    return total;
}

JoinResult executeJoin(Database& db) {
    optimizeAlignmentExecutionOrder(db);

    auto& tables = db.tables;

    if (tables.empty()) {
        return {};
    }

    if (tables.size() == 1) {
        JoinResult result = JoinResult::createFromNames(tables[0].names, tables[0].names);
        result.columns = tables[0].columns;
        return result;
    }

    auto pickAnAlgorithmAndJoin = [](const ColumnGroup& a,
                                     const ColumnGroup& b,
                                     const ColumnNames& aNames,
                                     const ColumnNames& bNames) -> JoinResult {
        i64 matchIdxA = -1;
        i64 matchIdxB = -1;
        for (u64 i = 0; i < aNames.colNames.size(); i++) {
            for (u64 j = 0; j < bNames.colNames.size(); j++) {
                if (aNames.colNames[i] == bNames.colNames[j]) {
                    matchIdxA = i;
                    matchIdxB = j;
                    break;
                }
            }
            if (matchIdxA >= 0) break;
        }

        if (matchIdxA == -1 && matchIdxB == -1) {
            if (a.size() > b.size()) {
                logInfo("Joining A and B with cross join.");
                return crossJoin(a, b, aNames, bNames);
            }
            else {
                logInfo("Joining B and A with cross join.");
                return crossJoin(b, a, aNames, bNames);
            }
        }
        else if (matchIdxA == 0 && matchIdxA == matchIdxB) {
            // Both have the same clustered index.
            // return mergeJoin(a, b, aNames, bNames);
            logInfo("Joining A and B with binary search join.");
            return binarySearchJoin(a, b, aNames, bNames);
        }
        else if (matchIdxA == 0 && matchIdxB > 0) {
            logInfo("Joining B and A with binary search join.");
            return binarySearchJoin(b, a, aNames, bNames);
        }
        else if (matchIdxB == 0 && matchIdxA > 0) {
            logInfo("Joining A and B with binary search join.");
            return binarySearchJoin(a, b, aNames, bNames);
        }
        else if (matchIdxA >= 1 && matchIdxB >= 1) {
            if (a.size() > b.size()) {
                logInfo("Joining A and B with hash join.");
                return hashJoin(a, b, aNames, bNames);
            }
            else {
                logInfo("Joining B and A with hash join.");
                return hashJoin(b, a, aNames, bNames);
            }
        }
        else {
            logFatal("Failed to pick a join algorithm.");
            Panic("[BUG] Failed to pick a join algorithm.");
            return {};
        }
    };

    JoinResult result = pickAnAlgorithmAndJoin(tables[0].columns, tables[1].columns, tables[0].names, tables[1].names);
    for (u64 i = 2; i < tables.size(); i++) {
        result = pickAnAlgorithmAndJoin(result.columns, tables[i].columns, result.names, tables[i].names);
    }

    return result;
}

void debug_printColumnGroup(const ColumnGroup& cols, const ColumnNames& columnNames) {
    printf("\nColumn Names:\n\n");

    // Print the column names
    for (u64 i = 0; i < columnNames.colNames.size(); i++) {
        printf("%s\t\t", columnNames.colNames[i].c_str());
    }

    // Print the value column names
    for (u64 i = 0; i < columnNames.valueColNames.size(); i++) {
        printf("%s\t\t", columnNames.valueColNames[i].c_str());
    }

    printf("\n\nValues:\n\n");

    // Print rows
    for (u64 i = 0; i < cols[0].data().size(); i++) {
        for (u64 j = 0; j < cols.size(); j++) {
            printf("%lu\t\t", cols[j].data()[i]);
        }
        printf("\n");
    }

    printf("\n");
}

} // namespace dbms
