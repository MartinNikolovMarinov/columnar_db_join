#include <dbms.h>

// #include "tools/tools.h"

std::vector<dbms::Table> optimizeExecutionOrder(const std::vector<dbms::Table>& tables) {
    std::vector<dbms::Table> result;
    result.reserve(tables.size());

    std::vector<i32> groups (tables.size());

    i32 nextGroupId = 1;

    // Group together all tables that have the same clustered index
    {
        for (addr_size i = 0; i < tables.size(); i++) {
            if (tables[i].columns.empty()) continue;

            const std::string& a = tables[i].columns[0].name;

            const auto it = std::find_if(tables.begin(), tables.end(), [&](const dbms::Table& table) -> bool {
                if (table.columns.empty()) return false;
                return table.columns[0].name == a;
            });
            if (it != tables.end()) {
                // Found table that has the same clustered index.

                addr_size idx = it - tables.begin();

                if (groups[idx] == 0 && groups[i] == 0) {
                    groups[idx] = nextGroupId;
                    groups[i] = nextGroupId;
                    nextGroupId++;
                }
                else if (groups[idx] == 0) {
                    groups[idx] = groups[i];
                }
                else if (groups[i] == 0) {
                    groups[i] = groups[idx];
                }
                else {
                    Assert(groups[i] == groups[idx]);
                }
            }
        }
    }

    // Group together all tables that can be joined on one cluster index
    {

    }

    // Group together with least priority tables that can be joined only on un-clustered indices:
    {

    }

    return result;
}

int main() {
    using namespace dbms;

    initSubmodules();

    std::vector<dbms::Table> tables(3);


    tables = optimizeExecutionOrder(tables);

    return 0;
}
