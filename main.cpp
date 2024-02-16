#include <dbms.h>

int main() {
    using namespace dbms;

    initSubmodules();

    std::error_code error;
    const char* p = "does not matter";

    std::vector<dbms::Table> tables;

    {
        dbms::Column c1 = { "A", mio::make_mmap_source(p, 0, mio::map_entire_file, error) };
        dbms::Column c2 = { "B", mio::make_mmap_source(p, 0, mio::map_entire_file, error) };
        dbms::Column c3 = { "C", mio::make_mmap_source(p, 0, mio::map_entire_file, error) };
        dbms::Table t;
        t.columns.push_back(std::move(c1));
        t.columns.push_back(std::move(c2));
        t.columns.push_back(std::move(c3));
        tables.push_back(std::move(t));
    }
    {
        dbms::Column c1 = { "F", mio::make_mmap_source(p, 0, mio::map_entire_file, error) };
        dbms::Table t;
        t.columns.push_back(std::move(c1));
        tables.push_back(std::move(t));
    }
    {
        dbms::Column c1 = { "B", mio::make_mmap_source(p, 0, mio::map_entire_file, error) };
        dbms::Column c2 = { "F", mio::make_mmap_source(p, 0, mio::map_entire_file, error) };
        dbms::Table t;
        t.columns.push_back(std::move(c1));
        t.columns.push_back(std::move(c2));
        tables.push_back(std::move(t));
    }

    if (isJoinPossible(tables)) {
        logInfo("Join is possible");
    }
    else {
        logFatal("Join is not possible");
        return -1;
    }

    tables = optimizeExecutionOrder(std::move(tables));

    return 0;
}
