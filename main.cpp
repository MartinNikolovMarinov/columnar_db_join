#include <dbms.h>

#include <unordered_map>
#include <optional>
#include <vector>

int main() {
    using namespace dbms;

    initSubmodules();

    auto group = dbms::ColumnGroup {
        dbms::Column { std::vector<u64> { 4, 86, 86, 86 } },
        dbms::Column { std::vector<u64> { 1, 1, 1, 2 } },
        dbms::Column { std::vector<u64> { 9, 9, 9, 6 } },
        dbms::Column { std::vector<u64> { 7, 11, 17, 10 } },
        dbms::Column { std::vector<u64> { 3, 7, 7, 0 } },
        dbms::Column { std::vector<u64> { 12, 12, 12, 5 } },
        dbms::Column { std::vector<u64> { 4, 19, 7, 3 } },
    };
    auto names = dbms::ColumnNames { { "A", "B", "C", "D" }, { "Column1", "Column2", "Column3" } };

    debug_printColumnGroup(group, names);

    return 0;
}
