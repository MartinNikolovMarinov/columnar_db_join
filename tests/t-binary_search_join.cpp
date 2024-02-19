#include "t-index.h"

#include "t-index.h"

#include <cmath>

i32 binarySearchJoinOnTwoColumnGroups() {
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
        auto result = dbms::binarySearchJoin(t.left, t.right, t.leftColNames, t.rightColNames);

        sortDataInColumns(result.columns); // TODO: is this necessary?

        Assert(result.names.colNames == t.expected.names.colNames, "Column names mismatch");
        Assert(result.names.valueColNames == t.expected.names.valueColNames, "Value column names mismatch");
        assertColumns(result.columns, t.expected.columns);

        u64 totalSumSquared = dbms::sumSquared(result);

        Assert(totalSumSquared == t.expectedSumSquared, "Sum squared mismatch");
        assertColumns(result.columns.back(), t.expectedSumSquaredCol);
    }

    return 0;
}

i32 binarySearchJoinOnMultipleColumnGroups() {
    struct TestCase {
        std::vector<dbms::ColumnGroup> sequence;
        std::vector<dbms::ColumnNames> sequenceColNames;
        dbms::JoinResult expected;
        dbms::Column expectedSumSquaredCol;
        u64 expectedSumSquared;
    };

    constexpr const u64 N = 1;
    std::vector<TestCase> tests (N);


    {
        // A   B   C   D   Column1 Column2 Column3 Sum^2
        // 42  1   9   7   3       12      4       361
        // 86  1   9   11  7       12      19      1444
        // 86  1   9   17  7       12      7       676
        // 86  2   6   10  0       5       3       64

        tests[0].sequence = {
            dbms::ColumnGroup {
                dbms::Column { std::vector<u64>{42, 42, 42, 42, 85, 86, 86} },
                dbms::Column { std::vector<u64>{0, 1, 7, 8, 1, 1, 2} },
                dbms::Column { std::vector<u64>{1, 3, 5, 5, 3, 7, 0} },
            },
            dbms::ColumnGroup {
                dbms::Column { std::vector<u64>{1, 2} },
                dbms::Column { std::vector<u64>{9, 6} },
                dbms::Column { std::vector<u64>{12, 5} },
            },
            dbms::ColumnGroup {
                dbms::Column { std::vector<u64>{ 42, 42, 86, 86, 86, 86, 86, 92 } },
                dbms::Column { std::vector<u64>{ 9, 10, 2, 6, 9, 9, 32, 7 } },
                dbms::Column { std::vector<u64>{ 7, 8, 9, 10, 11, 17, 12, 13 } },
                dbms::Column { std::vector<u64>{ 4, 5, 17, 3, 19, 7, 5, 5 } },
            },
        };
        tests[0].sequenceColNames = {
            dbms::ColumnNames { { "A", "B" }, { "value" } },
            dbms::ColumnNames { { "B", "C" }, { "value" } },
            dbms::ColumnNames { { "A", "C", "D" }, { "value" } },
        };

        tests[0].expected = dbms::JoinResult {
            dbms::ColumnGroup {
                dbms::Column { std::vector<u64> { 42, 86, 86, 86 } },
                dbms::Column { std::vector<u64> { 1, 1, 1, 2 } },
                dbms::Column { std::vector<u64> { 9, 9, 9, 6 } },
                dbms::Column { std::vector<u64> { 7, 11, 17, 10 } },
                dbms::Column { std::vector<u64> { 3, 7, 7, 0 } },
                dbms::Column { std::vector<u64> { 12, 12, 12, 5 } },
                dbms::Column { std::vector<u64> { 4, 19, 7, 3 } },
            },
            dbms::ColumnNames { { "A", "B", "C", "D" }, { "Column1", "Column2", "Column3" } },
        };
        tests[0].expectedSumSquaredCol = dbms::Column {
            std::vector<u64>{ 361, 1444, 676, 64 }
        };
        tests[0].expectedSumSquared = 2545;
    }

    for (auto& t : tests) {
        dbms::JoinResult result = dbms::binarySearchJoin(t.sequence[0], t.sequence[1], t.sequenceColNames[0], t.sequenceColNames[1]);
        for (u64 i = 2; i < t.sequence.size(); i++) {
            result = dbms::binarySearchJoin(result.columns, t.sequence[i], result.names, t.sequenceColNames[i]);
        }

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

i32 runBinarySearchJoinTestsSute() {
    RunTest(binarySearchJoinOnTwoColumnGroups);
    RunTest(binarySearchJoinOnMultipleColumnGroups);
    return 0;
}
