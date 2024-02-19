#include "t-index.h"

#include <cmath>

i32 crossJoinOnTwoColumnGroups() {
    struct TestCase {
        dbms::ColumnGroup left;
        dbms::ColumnGroup right;
        dbms::ColumnNames leftColNames;
        dbms::ColumnNames rightColNames;
        dbms::JoinResult expected;
        dbms::Column expectedSumSquaredCol;
        u64 expectedSumSquared;
    };

    constexpr const u64 N = 2;
    std::vector<TestCase> tests (N);

    {
        tests[0].left = {
            dbms::Column { std::vector<u64>{1, 2, 3} },
            dbms::Column { std::vector<u64>{11, 12, 13} },
        };
        tests[0].leftColNames.colNames = { "A" };
        tests[0].leftColNames.valueColNames = { "value" };

        tests[0].right = {
            dbms::Column { std::vector<u64>{4, 5} },
            dbms::Column { std::vector<u64>{14, 15} },
        };
        tests[0].rightColNames.colNames = { "B" };
        tests[0].rightColNames.valueColNames = { "value" };

        tests[0].expected = dbms::JoinResult {
            dbms::ColumnGroup {
                dbms::Column {
                    std::vector<u64> { 1, 1, 2, 2, 3, 3 }
                },
                dbms::Column {
                    std::vector<u64> { 4, 5, 4, 5, 4, 5 }
                },
                dbms::Column {
                    std::vector<u64> { 11, 11, 12, 12, 13, 13 }
                },
                dbms::Column {
                    std::vector<u64> { 14, 15, 14, 15, 14, 15 }
                },
            },
            dbms::ColumnNames { { "A", "B" }, { "Column1", "Column2" } },
        };

        tests[0].expectedSumSquaredCol = dbms::Column {
            std::vector<u64>{ 625, 676, 676, 729, 729, 784 }
        };
        tests[0].expectedSumSquared = 4219;
    }
    {
        tests[1].left = {
            dbms::Column { std::vector<u64>{1, 8} },
            dbms::Column { std::vector<u64>{2, 9} },
            dbms::Column { std::vector<u64>{3, 10} },
        };
        tests[1].leftColNames.colNames = { "A", "C" };
        tests[1].leftColNames.valueColNames = { "value" };

        tests[1].right = {
            dbms::Column { std::vector<u64>{4, 11} },
            dbms::Column { std::vector<u64>{5, 12} },
            dbms::Column { std::vector<u64>{6, 13} },
            dbms::Column { std::vector<u64>{7, 14} },
        };
        tests[1].rightColNames.colNames = { "B", "D", "E" };
        tests[1].rightColNames.valueColNames = { "value" };

        tests[1].expected = dbms::JoinResult {
            dbms::ColumnGroup {
                dbms::Column {
                    std::vector<u64> { 1, 1, 8, 8 }
                },
                dbms::Column {
                    std::vector<u64> { 4, 11, 4, 11 }
                },
                dbms::Column {
                    std::vector<u64> { 2, 2, 9, 9 }
                },
                dbms::Column {
                    std::vector<u64> { 5, 12, 5, 12 }
                },
                dbms::Column {
                    std::vector<u64> { 6, 13, 6, 13 }
                },
                dbms::Column {
                    std::vector<u64> { 3, 3, 10, 10 }
                },
                dbms::Column {
                    std::vector<u64> { 7, 14, 7, 14 }
                },
            },
            dbms::ColumnNames { { "A", "B", "C", "D", "E" }, { "Column1", "Column2" } },
        };

        tests[1].expectedSumSquaredCol = dbms::Column {
            std::vector<u64>{ 100, 289, 289, 576 }
        };
        tests[1].expectedSumSquared = 1254;
    }

    for (auto& t : tests) {
        auto result = dbms::crossJoin(t.left, t.right, t.leftColNames, t.rightColNames);

        sortDataInColumns(result.columns);

        Assert(result.names.colNames == t.expected.names.colNames, "Column names mismatch");
        Assert(result.names.valueColNames == t.expected.names.valueColNames, "Value column names mismatch");
        assertColumns(result.columns, t.expected.columns);

        u64 totalSumSquared = dbms::sumSquared(result);

        Assert(totalSumSquared == t.expectedSumSquared, "Sum squared mismatch");
        assertColumns(result.columns.back(), t.expectedSumSquaredCol);
    }

    return 0;
}

i32 runCrossJoinTestsSuite() {
    RunTest(crossJoinOnTwoColumnGroups);
    return 0;
}
