#include <dbms.h>

int main() {
    using namespace dbms;

    initSubmodules();

    Database db;
    loadDatabase(DBMS_DATA_PATH"/small-example", db);

    for (auto& table : db.tables) {
        debug_printTable(table);
    }

    return 0;
}
