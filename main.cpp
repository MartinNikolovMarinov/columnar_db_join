#include <dbms.h>

#include <unordered_map>
#include <optional>
#include <vector>

int main() {
    using namespace dbms;

    initSubmodules();

    TRACE_BLOCK_CPU_TIME("total");

    if (true) {

        // Cross join hardcoded example testing.

        constexpr u64 firstCount = 9000;
        dbms::ColumnGroup columns1 = {{}, {}, {}};
        ColumnNames names1;
        names1.colNames = { "A", "B" };
        names1.valueColNames = { "value" };

        for (u64 i = 0; i < firstCount; i++) {
            columns1[0].append(core::rndU64(0, 100000000));
            columns1[1].append(core::rndU64(0, 100000000));
            columns1[2].append(core::rndU64(0, 100000000));
        }

        constexpr u64 secondCount = 11000;
        dbms::ColumnGroup columns2 = {{}, {}};
        ColumnNames names2;
        names2.colNames = { "D" };
        names2.valueColNames = { "value" };

        for (u64 i = 0; i < secondCount; i++) {
            columns2[0].append(core::rndU64(0, 100000000));
            columns2[1].append(core::rndU64(0, 100000000));
        }

        dbms::JoinResult result = dbms::crossJoin(columns1, columns2, names1, names2);

        u64 totalSum = dbms::sumSquared(result);

        // debug_printColumnGroup(result.columns, result.names);
        logInfo("Total sum of squared values: %llu", totalSum);

        return 0;
    }

    Database db;
    if (!loadDatabase(DBMS_DATA_PATH"/t1-example", db)) {
        logFatal("Failed to load database");
        return 1;
    }

    // for (const auto& table : db.tables) {
    //     debug_printColumnGroup(table.columns, table.names);
    // }

    dbms::JoinResult result = dbms::binarySearchJoin(db.tables[0].columns, db.tables[1].columns, db.tables[0].names, db.tables[1].names);
    for (u64 i = 2; i < db.tables.size(); i++) {
        result = dbms::binarySearchJoin(result.columns, db.tables[i].columns, result.names, db.tables[i].names);
    }

    u64 totalSum = dbms::sumSquared(result);

    // debug_printColumnGroup(result.columns, result.names);
    logInfo("Total sum of squared values: %llu", totalSum);

    return 0;
}
