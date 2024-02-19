#include <dbms.h>

#include <unordered_map>
#include <optional>
#include <vector>

int main() {
    using namespace dbms;

    initSubmodules();

    Database db;
    if (!loadDatabase(DBMS_DATA_PATH"/small-example", db)) {
        logFatal("Failed to load database");
        return 1;
    }

    for (const auto& table : db.tables) {
        debug_printColumnGroup(table.columns, table.names);
    }

    return 0;
}
