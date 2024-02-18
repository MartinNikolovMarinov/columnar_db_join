#include "t-index.h"

void assertColumns(const dbms::ColumnGroup& a, const dbms::ColumnGroup& b) {
    for (u64 i = 0; i < a.size(); i++) {
        auto adata = a[i].data();
        auto bdata = b[i].data();
        Assert(adata.size() == bdata.size(), "Column size mismatch");
        for (u64 j = 0; j < adata.size(); j++) {
            Assert(adata[j] == bdata[j], "Column data mismatch");
        }
    }
}

i32 hashJoinOnTwoColumnGroups() {
     struct TestCase {
        dbms::ColumnGroup left;
        dbms::ColumnGroup right;
        dbms::ColumnNames leftColNames;
        dbms::ColumnNames rightColNames;
        dbms::JoinResult expected;
    };

    constexpr const u64 N = 1;
    std::vector<TestCase> tests (N);

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
    }

    for (auto& t : tests) {
        auto result = dbms::hashJoin(t.left, t.right, t.leftColNames, t.rightColNames);
        Assert(result.names.colNames == t.expected.names.colNames, "Column names mismatch");
        Assert(result.names.valueColNames == t.expected.names.valueColNames, "Value column names mismatch");
        assertColumns(result.columns, t.expected.columns);
    }

    return 0;
}

i32 runHashJoinTestsSuite() {
    RunTest(hashJoinOnTwoColumnGroups);
    return 0;
}
