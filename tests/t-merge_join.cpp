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

    constexpr const u64 N = 1;
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

    for (auto& t : tests) {
        auto result = dbms::mergeJoin(t.left, t.right, t.leftColNames, t.rightColNames);

        // sortDataInColumns(result.columns);

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
