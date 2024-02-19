#include "t-index.h"

i32 hashJoinOnTwoColumnGroups() {
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

    // Test Case 1
    {
        tests[0].left = {
            dbms::Column { std::vector<u64>{1, 2, 3, 4, 5, 6, 7} },
            dbms::Column { std::vector<u64>{1, 1, 1, 2, 3, 2, 1} },
            dbms::Column { std::vector<u64>{11, 12, 13, 14, 15, 16, 17} },
        };
        tests[0].leftColNames.colNames = { "A", "C" };
        tests[0].leftColNames.valueColNames = { "value" };

        tests[0].right = {
            dbms::Column { std::vector<u64>{8, 9, 10, 11, 12, 13, 14, 15} },
            dbms::Column { std::vector<u64>{1, 2, 1, 2, 3, 2, 1, 1} },
            dbms::Column { std::vector<u64>{18, 19, 20, 21, 22, 23, 24, 25} },
        };
        tests[0].rightColNames.colNames = { "B", "C" };
        tests[0].rightColNames.valueColNames = { "value" };

        tests[0].expected = dbms::JoinResult {
            dbms::ColumnGroup {
                dbms::Column {
                    std::vector<u64> { 1, 1, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 4, 4, 4, 5, 6, 6, 6, 7, 7, 7, 7 }
                },
                dbms::Column {
                    std::vector<u64> { 8, 10, 14, 15, 8, 10, 14, 15, 8, 10, 14, 15, 9, 11, 13, 12, 9, 11, 13, 8, 10, 14, 15 }
                },
                dbms::Column {
                    std::vector<u64> { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 3, 2, 2, 2, 1, 1, 1, 1 }
                },
                dbms::Column {
                    std::vector<u64> { 11, 11, 11, 11, 12, 12, 12, 12, 13, 13, 13, 13, 14, 14, 14, 15, 16, 16, 16, 17, 17, 17, 17 }
                },
                dbms::Column {
                    std::vector<u64> { 18, 20, 24, 25, 18, 20, 24, 25, 18, 20, 24, 25, 19, 21, 23, 22, 19, 21, 23, 18, 20, 24, 25 }
                },
            },
            dbms::ColumnNames { { "A", "B", "C" }, { "Column1", "Column2" } },
        };

        tests[0].expectedSumSquaredCol = dbms::Column {
            std::vector<u64>{ 841, 961, 1225, 1296, 900, 1024, 1296, 1369, 961, 1089, 1369, 1444, 1089, 1225, 1369, 1369, 1225, 1369, 1521, 1225, 1369, 1681, 1764 }
        };
        tests[0].expectedSumSquared = 28981;
    }

    // Test Case 2
    {
        tests[1].left = {
            dbms::Column { std::vector<u64>{10, 21, 32, 43, 54, 65, 76, 77, 18, 29, 310, 411, 512, 613, 714, 715} },
            dbms::Column { std::vector<u64>{1, 1, 2, 3, 2, 4, 1, 2, 1, 1, 1, 1, 2, 3, 2, 4} },
            dbms::Column { std::vector<u64>{2, 1, 1, 4, 1, 2, 3, 1, 1, 1, 3, 2, 4, 2, 2, 4} },
            dbms::Column { std::vector<u64>{11, 22, 33, 44, 55, 66, 77, 88, 99, 1010, 1111, 1212, 1313, 1414, 1515, 1616} },
        };
        tests[1].leftColNames.colNames = { "A", "C", "E" };
        tests[1].leftColNames.valueColNames = { "value" };

        tests[1].right = {
            dbms::Column { std::vector<u64>{89, 71, 74, 8, 16, 70, 65, 33, 94, 23, 90, 90, 42, 93, 77, 24} },
            dbms::Column { std::vector<u64>{3, 4, 3, 4, 2, 4, 1, 3, 4, 3, 1, 1, 1, 1, 2, 4} },
            dbms::Column { std::vector<u64>{1, 4, 1, 3, 2, 4, 4, 4, 1, 1, 3, 1, 1, 2, 2, 1} },
            dbms::Column { std::vector<u64>{144, 104, 16, 121, 44, 151, 33, 143, 51, 117, 198, 157, 120, 10, 46, 115} },
        };
        tests[1].rightColNames.colNames = { "B", "C", "E" };
        tests[1].rightColNames.valueColNames = { "value" };

        tests[1].expected = dbms::JoinResult {
            dbms::ColumnGroup {
                dbms::Column {
                    std::vector<u64> { 10, 18, 18, 21, 21, 29, 29, 43, 76, 310, 411, 714, 714, 715, 715 },
                },
                dbms::Column {
                    std::vector<u64> { 93, 42, 90, 42, 90, 42, 90, 33, 90, 90, 93, 16, 77, 70, 71 },
                },
                dbms::Column {
                    std::vector<u64> { 1, 1, 1, 1, 1, 1, 1, 3, 1, 1, 1, 2, 2, 4, 4 },
                },
                dbms::Column {
                    std::vector<u64> { 2, 1, 1, 1, 1, 1, 1, 4, 3, 3, 2, 2, 2, 4, 4 },
                },
                dbms::Column {
                    std::vector<u64> { 11, 99, 99, 22, 22, 1010, 1010, 44, 77, 1111, 1212, 1515, 1515, 1616, 1616 },
                },
                dbms::Column {
                    std::vector<u64> { 10, 120, 157, 120, 157, 120, 157, 143, 198, 198, 10, 44, 46, 151, 104 },
                },
            },
            dbms::ColumnNames { { "A", "B", "C", "E" }, { "Column1", "Column2" } },
        };

        tests[1].expectedSumSquaredCol = dbms::Column {
            std::vector<u64>{441, 47961, 65536, 20164, 32041, 1276900, 1361889, 34969, 75625, 1713481, 1493284, 2430481, 2436721, 3122289, 2958400}
        };
        tests[1].expectedSumSquared = 17070182;
    }

    for (auto& t : tests) {
        auto result = dbms::hashJoin(t.left, t.right, t.leftColNames, t.rightColNames);

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

i32 hashJoinOnMultipleColumnGroups() {
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
        dbms::JoinResult result = dbms::hashJoin(t.sequence[0], t.sequence[1], t.sequenceColNames[0], t.sequenceColNames[1]);
        for (u64 i = 2; i < t.sequence.size(); i++) {
            result = dbms::hashJoin(result.columns, t.sequence[i], result.names, t.sequenceColNames[i]);
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

i32 runHashJoinTestsSuite() {
    RunTest(hashJoinOnTwoColumnGroups);
    RunTest(hashJoinOnMultipleColumnGroups);
    return 0;
}
