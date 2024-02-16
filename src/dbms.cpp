#include <dbms.h>

namespace dbms {

void initSubmodules() {
    core::initLoggingSystem(core::LogLevel::L_TRACE, nullptr, 0);
    core::rndInit();

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

} // namespace dbms

