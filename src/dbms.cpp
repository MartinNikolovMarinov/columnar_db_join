#include <dbms.h>

#include <unordered_set>
#include <stdio.h>
#include <string.h>

namespace dbms {

void initSubmodules() {
    core::initLoggingSystem(core::LogLevel::L_TRACE, nullptr, 0);
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

// void loadDatabase(const char* path, Database& db) {
//     namespace fs = std::filesystem;

//     db.path = path;
//     db.name = fs::path(path).filename().string();
//     db.tables.clear();

//     for (const auto & columnDir : fs::directory_iterator(db.path)) {
//         if (columnDir.is_directory()) {
//             Table t;
//             t.name = columnDir.path().stem().string();
//             t.path = columnDir.path();

//             for (const auto & columnFile : fs::directory_iterator(columnDir.path())) {
//                 if (columnFile.is_regular_file()) {
//                     std::string colName = columnFile.path().stem().string();
//                     if (colName != "value" && colName.find(DBMS_DATA_COLUMN_PREFIX) != std::string::npos) {
//                         colName = colName.erase(0, colName.find_last_of(DBMS_DATA_COLUMN_PREFIX) + 1);
//                     }
//                     t.columnNames.push_back(std::move(colName));
//                 }
//             }

//             std::sort(t.columnNames.begin(), t.columnNames.end(), [](const std::string& a, const std::string& b) {
//                 if (a == "value") return false;
//                 return a < b;
//             });

//             for (const auto& colName : t.columnNames) {
//                 std::string pathToFile;
//                 if (colName == "value") {
//                     pathToFile = (columnDir.path() / (colName + DBMS_DATA_FILE_EXTENSION)).string();
//                 }
//                 else {
//                     pathToFile = (columnDir.path() / (DBMS_DATA_COLUMN_PREFIX + colName + DBMS_DATA_FILE_EXTENSION)).string();
//                 }

//                 ColumnData c;
//                 int error = c.memorySrc.open(pathToFile.c_str(), 0);
//                 if (error != 0) {
//                     logFatal("Error Mapping File: %s, exiting...", strerror(error));
//                     Panic(false, "Failed to loadDatabase. Reason: Mapping failed.");
//                 }
//                 t.columns.push_back(std::move(c));
//             }

//             db.tables.push_back(std::move(t));
//         }
//     }
// }

// void debug_printTable(dbms::Table& table) {
//     std::vector<std::vector<u64>> inmemory;
//     std::vector<std::string> columnNames;

//     for (addr_size i = 0; i < table.columns.size(); i++) {
//         auto& col = table.columns[i];
//         const auto& colName = table.columnNames[i];

//         std::vector<u64> columnData;
//         columnData.reserve(col.memorySrc.size() / sizeof(u64));
//         inmemory.push_back(columnData);

//         columnNames.push_back(colName);

//         addr_size max = 0;
//         const u64* data = static_cast<const u64*>(col.memorySrc.data(0, max));
//         Assert(max == col.memorySrc.size(), "TODO: Implement reading multiple chunks of data for data sources that don't use memory mapping.");
//         max /= sizeof(u64);

//         for (addr_size j = 0; j < max; j++) {
//             inmemory[i].push_back(data[j]);
//         }
//     }

//     std::string result;

//     // Print table header:
//     for (addr_size j = 0; j < columnNames.size(); j++) {
//         result += columnNames[j] + " ";
//     }
//     result += "\n";

//     // Print table data:
//     for (addr_size i = 0; i < inmemory[0].size(); i++) {
//         for (addr_size j = 0; j < inmemory.size(); j++) {
//             result += std::to_string(inmemory[j][i]) + " ";
//         }
//         result += "\n";
//     }

//     logInfo("\nTable: %s\n%s", table.name.c_str(), result.c_str());
// }

// std::vector<dbms::Table> optimizeAlignmentExecutionOrder(std::vector<dbms::Table>&& tables) {
//     if (tables.empty()) return {};

//     std::vector<dbms::Table> result;
//     result.reserve(tables.size());

//     std::unordered_set<addr_size> joinWithBothClustered;
//     std::unordered_set<addr_size> joinWithOneClustered;
//     std::unordered_set<addr_size> joinWithNoClustered;

//     // Group together all tables that have the same clustered index
//     {
//         for (addr_size i = 0; i < tables.size(); i++) {
//             if (tables[i].columnNames.empty()) continue;

//             const std::string& a = tables[i].columnNames[0];

//             addr_off idx = -1;
//             for (addr_size j = 0; j < tables.size(); j++) {
//                 if (i == j) continue;
//                 if (tables[j].columnNames.empty()) continue;

//                 const std::string& b = tables[j].columnNames[0];
//                 if (a == b) {
//                     idx = j;
//                     break;
//                 }
//             }

//             if (idx >= 0) {
//                 joinWithBothClustered.insert(i);
//             }
//         }
//     }

//     // Group together all tables that can be joined on one cluster index
//     {
//         for (addr_size i = 0; i < tables.size(); i++) {
//             if (tables[i].columnNames.empty()) continue;
//             if (joinWithBothClustered.count(i) > 0) continue;

//             const std::string& a = tables[i].columnNames[0];

//             addr_off idx = -1;
//             for (addr_size j = 0; j < tables.size(); j++) {
//                 if (i == j) continue;
//                 if (tables[j].columnNames.empty()) continue;

//                 for (const auto& col : tables[j].columnNames) {
//                     if (a == col) {
//                         idx = j;
//                         break;
//                     }
//                 }

//                 if (idx >= 0) break;
//             }

//             if (idx >= 0) {
//                 joinWithOneClustered.insert(i);
//             }
//         }
//     }

//     // Group together with least priority tables that can be joined only on un-clustered indices:
//     {
//         for (addr_size i = 0; i < tables.size(); i++) {
//             if (tables[i].columnNames.empty()) continue;

//             if (joinWithBothClustered.count(i) == 0 && joinWithOneClustered.count(i) == 0) {
//                 joinWithNoClustered.insert(i);
//             }
//         }
//     }

//     // Add the tables to the result in the order of their priority:

//     for (addr_size i : joinWithBothClustered) {
//         result.push_back(std::move(tables[i]));
//     }

//     for (addr_size i : joinWithOneClustered) {
//         result.push_back(std::move(tables[i]));
//     }

//     for (addr_size i : joinWithNoClustered) {
//         result.push_back(std::move(tables[i]));
//     }

//     return result;
// }

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

} // namespace dbms
