#include <dbms.h>

#include <unordered_map>
#include <optional>
#include <vector>

int main() {
    using namespace dbms;

    initSubmodules(core::LogLevel::L_INFO);

    INFO_BLOCK_CPU_TIME("total");

    Database db;
    if (!loadDatabase(DBMS_DATA_PATH"/t1-example", db)) {
        logFatal("Failed to load database");
        return 1;
    }

    dbms::JoinResult result = dbms::executeJoin(db);

    u64 totalSum = dbms::sumSquared(result);

    // debug_printColumnGroup(result.columns, result.names);
    logInfo("Total sum of squared values: %llu", totalSum);

    return 0;
}
