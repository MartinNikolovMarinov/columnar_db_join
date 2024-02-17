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

void loadDatabase(const char* path, Database& db) {
    namespace fs = std::filesystem;

    db.path = path;
    db.name = fs::path(path).filename().string();
    db.tables.clear();

    for (const auto & columnDir : fs::directory_iterator(db.path)) {
        if (columnDir.is_directory()) {
            Table t;
            t.name = columnDir.path().stem().string();
            t.path = columnDir.path();

            for (const auto & columnFile : fs::directory_iterator(columnDir.path())) {
                if (columnFile.is_regular_file()) {
                    std::string colName = columnFile.path().stem().string();
                    if (colName != "value" && colName.find(DBMS_DATA_COLUMN_PREFIX) != std::string::npos) {
                        colName = colName.erase(0, colName.find_last_of(DBMS_DATA_COLUMN_PREFIX) + 1);
                    }
                    t.columnNames.push_back(std::move(colName));
                }
            }

            std::sort(t.columnNames.begin(), t.columnNames.end(), [](const std::string& a, const std::string& b) {
                if (a == "value") return false;
                return a < b;
            });

            for (const auto& colName : t.columnNames) {
                std::string pathToFile;
                if (colName == "value") {
                    pathToFile = (columnDir.path() / (colName + DBMS_DATA_FILE_EXTENSION)).string();
                }
                else {
                    pathToFile = (columnDir.path() / (DBMS_DATA_COLUMN_PREFIX + colName + DBMS_DATA_FILE_EXTENSION)).string();
                }

                ColumnData c;
                int error = c.memorySrc.open(pathToFile.c_str(), 0);
                if (error != 0) {
                    logFatal("Error Mapping File: %s, exiting...", strerror(error));
                    Panic(false, "Failed to loadDatabase. Reason: Mapping failed.");
                }
                t.columns.push_back(std::move(c));
            }

            db.tables.push_back(std::move(t));
        }
    }
}

void debug_printTable(dbms::Table& table) {
    std::vector<std::vector<u64>> inmemory;
    std::vector<std::string> columnNames;

    for (addr_size i = 0; i < table.columns.size(); i++) {
        auto& col = table.columns[i];
        const auto& colName = table.columnNames[i];

        std::vector<u64> columnData;
        columnData.reserve(col.memorySrc.size() / sizeof(u64));
        inmemory.push_back(columnData);

        columnNames.push_back(colName);

        addr_size max = 0;
        const u64* data = static_cast<const u64*>(col.memorySrc.data(0, max));
        Assert(max == col.memorySrc.size(), "TODO: Implement reading multiple chunks of data for data sources that don't use memory mapping.");
        max /= sizeof(u64);

        for (addr_size j = 0; j < max; j++) {
            inmemory[i].push_back(data[j]);
        }
    }

    std::string result;

    // Print table header:
    for (addr_size j = 0; j < columnNames.size(); j++) {
        result += columnNames[j] + " ";
    }
    result += "\n";

    // Print table data:
    for (addr_size i = 0; i < inmemory[0].size(); i++) {
        for (addr_size j = 0; j < inmemory.size(); j++) {
            result += std::to_string(inmemory[j][i]) + " ";
        }
        result += "\n";
    }

    logInfo("\nTable: %s\n%s", table.name.c_str(), result.c_str());
}

std::vector<dbms::Table> optimizeExecutionOrder(std::vector<dbms::Table>&& tables) {
    if (tables.empty()) return {};

    std::vector<dbms::Table> result;
    result.reserve(tables.size());

    std::unordered_set<addr_size> joinWithBothClustered;
    std::unordered_set<addr_size> joinWithOneClustered;
    std::unordered_set<addr_size> joinWithNoClustered;

    // Group together all tables that have the same clustered index
    {
        for (addr_size i = 0; i < tables.size(); i++) {
            if (tables[i].columnNames.empty()) continue;

            const std::string& a = tables[i].columnNames[0];

            addr_off idx = -1;
            for (addr_size j = 0; j < tables.size(); j++) {
                if (i == j) continue;
                if (tables[j].columnNames.empty()) continue;

                const std::string& b = tables[j].columnNames[0];
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
        for (addr_size i = 0; i < tables.size(); i++) {
            if (tables[i].columnNames.empty()) continue;
            if (joinWithBothClustered.count(i) > 0) continue;

            const std::string& a = tables[i].columnNames[0];

            addr_off idx = -1;
            for (addr_size j = 0; j < tables.size(); j++) {
                if (i == j) continue;
                if (tables[j].columnNames.empty()) continue;

                for (const auto& col : tables[j].columnNames) {
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
        for (addr_size i = 0; i < tables.size(); i++) {
            if (tables[i].columnNames.empty()) continue;

            if (joinWithBothClustered.count(i) == 0 && joinWithOneClustered.count(i) == 0) {
                joinWithNoClustered.insert(i);
            }
        }
    }

    // Add the tables to the result in the order of their priority:

    for (addr_size i : joinWithBothClustered) {
        result.push_back(std::move(tables[i]));
    }

    for (addr_size i : joinWithOneClustered) {
        result.push_back(std::move(tables[i]));
    }

    for (addr_size i : joinWithNoClustered) {
        result.push_back(std::move(tables[i]));
    }

    return result;
}

bool isJoinPossible(const std::vector<dbms::Table>& tables) {
    // Iterate through each table to check if it has at least one column that matches a column in any other table
    for (size_t i = 0; i < tables.size(); ++i) {
        bool foundMatch = false;
        for (size_t j = 0; j < tables.size(); ++j) {
            if (i == j) continue; // Skip comparing the table with itself

            const auto& table1 = tables[i];
            const auto& table2 = tables[j];
            for (const auto& col1 : table1.columnNames) {
                for (const auto& col2 : table2.columnNames) {
                    if (col1 == col2) {
                        foundMatch = true;
                        break;
                    }
                }
                if (foundMatch) break;
            }
            if (foundMatch) break;
        }

        if (!foundMatch) return false; // If no matching column found for this table, join is not possible
    }

    return true; // All tables have at least one matching column with another table, join is possible
}

} // namespace dbms

