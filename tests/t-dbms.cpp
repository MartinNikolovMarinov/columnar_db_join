#include "t-index.h"

i32 testIndexTranslationTable() {
    struct TestCase {
        dbms::ColumnNames from;
        dbms::ColumnNames to;
        dbms::IndexTranslationTable expected;
    };

    TestCase tests[] = {
        {
            dbms::ColumnNames { { "A", "C", "E" }, {} },
            dbms::ColumnNames { { "A", "B", "C", "E" }, {} },
            dbms::IndexTranslationTable { {0, 0}, {1, 2}, {2, 3} },
        },
        {
            dbms::ColumnNames { { "A", "B", "C" }, {} },
            dbms::ColumnNames { { "A", "B" }, {} },
            dbms::IndexTranslationTable { {0, 0}, {1, 1} },
        },
        {
            dbms::ColumnNames { { "A", "B", "C" }, {} },
            dbms::ColumnNames { {}, {} },
            dbms::IndexTranslationTable {},
        },
        {
            dbms::ColumnNames { {}, {} },
            dbms::ColumnNames { { "A", "B", "C" }, {} },
            dbms::IndexTranslationTable {},
        },
        {
            dbms::ColumnNames { { "A", "B", "C" }, {} },
            dbms::ColumnNames { { "A", "B", "C" }, {} },
            dbms::IndexTranslationTable { {0, 0}, {1, 1}, {2, 2} },
        },
        {
            dbms::ColumnNames { { "B", "D", "F" }, {} },
            dbms::ColumnNames { { "A", "B", "C", "D" }, {} },
            dbms::IndexTranslationTable { {0, 1}, { 1, 3 } },
        },
        {
            dbms::ColumnNames { { "A", "B", "C", "D" }, {} },
            dbms::ColumnNames { { "D" }, {} },
            dbms::IndexTranslationTable { {3, 0} },
        },
        {
            dbms::ColumnNames { { "D" }, {} },
            dbms::ColumnNames { { "A", "B", "C", "D" }, {} },
            dbms::IndexTranslationTable { {0, 3} },
        },
    };

    for (auto& t : tests) {
        auto result = dbms::createIndexTranslationTable(t.from, t.to);
        Assert(result == t.expected, "Index translation table is not as expected.");
    }

    return 0;
}

i32 runDBMSTestsSuite() {
    RunTest(testIndexTranslationTable);
    return 0;
}
