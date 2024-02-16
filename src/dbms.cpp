#include <dbms.h>

#include <unordered_set>
#include <stdio.h>

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

    std::vector<Table> tables;

    for (const auto & columnDir : fs::directory_iterator(db.path)) {
        if (columnDir.is_directory()) {
            Table t;
            t.name = columnDir.path().stem().string();
            t.path = columnDir.path();

            for (const auto & columnFile : fs::directory_iterator(columnDir.path())) {
                if (columnFile.is_regular_file()) {
                    Column c;
                    c.name = columnFile.path().stem().string();
                    std::error_code error;
                    c.memorySrc = mio::make_mmap_source(columnFile.path().string().c_str(), 0, mio::map_entire_file, error);
                    if (error) {
                        logFatal("error mapping file: %s, exiting...", error.message().c_str());
                        Panic(false, "Mapping failed. Exiting...");
                    }
                    t.columns.push_back(std::move(c));
                }
            }

            tables.push_back(std::move(t));
        }
    }

    for (const auto& t : tables) {
        printf("Table: { name: %s, path: %s }\n", t.name.c_str(), t.path.c_str());
        for (const auto& c : t.columns) {
            printf("Column: { name: %s, size: %lu }\n", c.name.c_str(), c.memorySrc.size() / sizeof(u64));

            const u64* curr = reinterpret_cast<const u64*>(c.memorySrc.data());

            for (size_t i = 0; i < c.memorySrc.size() / sizeof(u64); i ++) {
                u64 e1 = *(curr);
                curr++;

                printf("%lu\n", e1);
            }
        }
    }
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
            if (tables[i].columns.empty()) continue;

            const std::string& a = tables[i].columns[0].name;

            addr_off idx = -1;
            for (addr_size j = 0; j < tables.size(); j++) {
                if (i == j) continue;
                if (tables[j].columns.empty()) continue;

                const std::string& b = tables[j].columns[0].name;
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
            if (tables[i].columns.empty()) continue;
            if (joinWithBothClustered.count(i) > 0) continue;

            const std::string& a = tables[i].columns[0].name;

            addr_off idx = -1;
            for (addr_size j = 0; j < tables.size(); j++) {
                if (i == j) continue;
                if (tables[j].columns.empty()) continue;

                for (const auto& col : tables[j].columns) {
                    if (a == col.name) {
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
            if (tables[i].columns.empty()) continue;

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
            for (const auto& col1 : table1.columns) {
                for (const auto& col2 : table2.columns) {
                    if (col1.name == col2.name) {
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

