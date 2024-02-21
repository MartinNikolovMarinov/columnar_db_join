#include <dbms.h>

#include <unordered_map>
#include <optional>
#include <vector>
#include <filesystem>

i32 main(i32 argc, char *argv[]) {
    using namespace dbms;

    initSubmodules(core::LogLevel::L_INFO);

    if (argc < 2) {
        logFatal("Usage: %s <database_directory>", argv[0]);
        return -1;
    }

    std::string dbDirectory = argv[1];
    if (!std::filesystem::exists(dbDirectory)) {
        logFatal("Database directory does not exist: %s", dbDirectory.c_str());
        return -2;
    }

    INFO_BLOCK_CPU_TIME("Total running time");

    Database db;
    if (!loadDatabase(dbDirectory.c_str(), db)) {
        logFatal("Failed to load database");
        return -3;
    }

    dbms::JoinResult result = dbms::executeJoin(db);

    u64 totalSum = dbms::sumSquared(result);

    debug_printColumnGroup(result.columns, result.names);
    printf("\nTotal sum of squared values: %llu\n\n", totalSum);

    return 0;
}
