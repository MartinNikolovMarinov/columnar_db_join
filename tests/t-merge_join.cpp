#include "t-index.h"

i32 mergeJoinOnTwoColumnGroups() {
    struct TestCase {
        dbms::ColumnGroup left;
        dbms::ColumnGroup right;
        dbms::ColumnNames leftColNames;
        dbms::ColumnNames rightColNames;
        dbms::JoinResult expected;
        dbms::Column expectedSumSquaredCol;
        u64 expectedSumSquared;
    };

    constexpr const u64 N = 3;
    std::vector<TestCase> tests (N);

    {
        tests[0].left = {
            dbms::Column { std::vector<u64>{1, 1, 1, 3} },
            dbms::Column { std::vector<u64>{2, 3, 7, 8} },
            dbms::Column { std::vector<u64>{11, 12, 13, 14} },
        };
        tests[0].leftColNames.colNames = { "A", "B" };
        tests[0].leftColNames.valueColNames = { "value" };

        tests[0].right = {
            dbms::Column { std::vector<u64>{1, 1, 2, 3} },
            dbms::Column { std::vector<u64>{2, 3, 4, 5} },
            dbms::Column { std::vector<u64>{15, 16, 17, 18} },
        };
        tests[0].rightColNames.colNames = { "A", "C" };
        tests[0].rightColNames.valueColNames = { "value" };

        tests[0].expected = dbms::JoinResult {
            dbms::ColumnGroup {
                dbms::Column {
                    std::vector<u64> {1, 1, 1, 1, 1, 1, 3}
                },
                dbms::Column {
                    std::vector<u64> {2, 2, 3, 3, 7, 7, 8}
                },
                dbms::Column {
                    std::vector<u64> {2, 3, 2, 3, 2, 3, 5}
                },
                dbms::Column {
                    std::vector<u64> {11, 11, 12, 12, 13, 13, 14}
                },
                dbms::Column {
                    std::vector<u64> {15, 16, 15, 16, 15, 16, 18}
                },
            },
            dbms::ColumnNames { { "A", "B", "C" }, { "Column1", "Column2" } },
        };
        tests[0].expectedSumSquaredCol = dbms::Column {
            std::vector<u64>{ 676, 729, 729, 784, 784, 841, 1024 }
        };
        tests[0].expectedSumSquared = 5567;
    }

    {
        tests[1].left = {
            dbms::Column { std::vector<u64>{1, 1, 1, 2, 3, 3} },
            dbms::Column { std::vector<u64>{1, 2, 2, 3, 4, 4} },
            dbms::Column { std::vector<u64>{7, 8, 9, 10, 11, 11} },
            dbms::Column { std::vector<u64>{12, 13, 14, 15, 16, 16} },
        };
        tests[1].leftColNames.colNames = { "A", "B", "C" };
        tests[1].leftColNames.valueColNames = { "value" };

        tests[1].right = {
            dbms::Column { std::vector<u64>{1, 1, 1, 1, 2, 3, 3} },
            dbms::Column { std::vector<u64>{1, 1, 2, 2, 3, 5, 5} },
            dbms::Column { std::vector<u64>{17, 18, 19, 20, 21, 22, 22} },
            dbms::Column { std::vector<u64>{23, 24, 25, 26, 27, 28, 28} },
        };
        tests[1].rightColNames.colNames = { "A", "B", "D" };
        tests[1].rightColNames.valueColNames = { "value" };

        tests[1].expected = dbms::JoinResult {
            dbms::ColumnGroup {
                dbms::Column {
                    std::vector<u64> {1, 1, 1, 1, 1, 1, 2}
                },
                dbms::Column {
                    std::vector<u64> {1, 1, 2, 2, 2, 2, 3}
                },
                dbms::Column {
                    std::vector<u64> {7, 7, 8, 8, 9, 9, 10}
                },
                dbms::Column {
                    std::vector<u64> {17, 18, 19, 20, 19, 20, 21}
                },
                dbms::Column {
                    std::vector<u64> {12, 12, 13, 13, 14, 14, 15}
                },
                dbms::Column {
                    std::vector<u64> {23, 24, 25, 26, 25, 26, 27}
                },
            },
            dbms::ColumnNames { { "A", "B", "C", "D" }, { "Column1", "Column2" } },
        };
        tests[1].expectedSumSquaredCol = dbms::Column {
            std::vector<u64>{1225, 1296, 1444, 1521, 1521, 1600, 1764}
        };
        tests[1].expectedSumSquared = 10371;
    }

    {
        tests[2].left = {
            dbms::Column { std::vector<u64>{ 1, 1, 1, 1, 2, 2, 3, 3, 3} },
            dbms::Column { std::vector<u64>{ 10, 11, 12, 13, 14, 15, 16, 17, 18} },
            dbms::Column { std::vector<u64>{ 4, 4, 5, 6, 6, 6, 4, 5, 5} },
            dbms::Column { std::vector<u64>{ 14, 14, 14, 14, 14, 14, 14, 14, 14} },
        };
        tests[2].leftColNames.colNames = { "A", "B", "E" };
        tests[2].leftColNames.valueColNames = { "value" };

        tests[2].right = {
            dbms::Column { std::vector<u64>{ 1, 1, 1, 2, 2, 3, 3} },
            dbms::Column { std::vector<u64>{ 4, 4, 5, 6, 6, 3, 4} },
            dbms::Column { std::vector<u64>{ 19, 20, 21, 22, 23, 24, 25} },
            dbms::Column { std::vector<u64>{ 26, 27, 28, 29, 30, 31, 32} },
            dbms::Column { std::vector<u64>{ 33, 34, 35, 36, 37, 38, 39} },
        };
        tests[2].rightColNames.colNames = { "A", "E", "G", "H" };
        tests[2].rightColNames.valueColNames = { "value" };

        tests[2].expected = dbms::JoinResult {
            dbms::ColumnGroup {
                dbms::Column { std::vector<u64>{ 1, 1, 1, 1, 1, 2, 2, 2, 2, 3 } },
                dbms::Column { std::vector<u64>{ 10, 10, 11, 11, 12, 14, 14, 15, 15, 16 } },
                dbms::Column { std::vector<u64>{ 4, 4, 4, 4, 5, 6, 6, 6, 6, 4 } },
                dbms::Column { std::vector<u64>{ 19, 20, 19, 20, 21, 22, 23, 22, 23, 25 } },
                dbms::Column { std::vector<u64>{ 26, 27, 26, 27, 28, 29, 30, 29, 30, 32 } },
                dbms::Column { std::vector<u64>{ 14, 14, 14, 14, 14, 14, 14, 14, 14, 14 } },
                dbms::Column { std::vector<u64>{ 33, 34, 33, 34, 35, 36, 37, 36, 37, 39 } },
            },
            dbms::ColumnNames { { "A", "B", "E", "G", "H" }, { "Column1", "Column2" } },
        };
        tests[2].expectedSumSquaredCol = dbms::Column {
            std::vector<u64>{2209, 2304, 2209, 2304, 2401, 2500, 2601, 2500, 2601, 2809}
        };
        tests[2].expectedSumSquared = 24438;
    }

    for (auto& t : tests) {
        auto result = dbms::mergeJoin(t.left, t.right, t.leftColNames, t.rightColNames);

        Assert(result.names.colNames == t.expected.names.colNames, "Column names mismatch");
        Assert(result.names.valueColNames == t.expected.names.valueColNames, "Value column names mismatch");
        assertColumns(result.columns, t.expected.columns);

        u64 totalSumSquared = dbms::sumSquared(result);

        Assert(totalSumSquared == t.expectedSumSquared, "Sum squared mismatch");
        assertColumns(result.columns.back(), t.expectedSumSquaredCol);
    }

    return 0;
}

i32 runMergeJoinTestsSuite() {
    RunTest(mergeJoinOnTwoColumnGroups);
    return 0;
}
