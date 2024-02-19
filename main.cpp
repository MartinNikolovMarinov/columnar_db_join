#include <dbms.h>

#include <unordered_map>
#include <optional>
#include <vector>

int main() {
    using namespace dbms;

    initSubmodules();

    Database db;
    if (!loadDatabase(DBMS_DATA_PATH"/t1-example", db)) {
        logFatal("Failed to load database");
        return 1;
    }

    // for (const auto& table : db.tables) {
    //     debug_printColumnGroup(table.columns, table.names);
    // }

    dbms::JoinResult result = dbms::mergeJoin(db.tables[0].columns, db.tables[1].columns, db.tables[0].names, db.tables[1].names);
    for (u64 i = 2; i < db.tables.size(); i++) {
        result = dbms::binarySearchJoin(result.columns, db.tables[i].columns, result.names, db.tables[i].names);
    }

    u64 totalSum = dbms::sumSquared(result);

    // debug_printColumnGroup(result.columns, result.names);
    logInfo("Total sum of squared values: %llu", totalSum);

    return 0;
}
