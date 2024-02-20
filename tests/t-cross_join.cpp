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

    constexpr const u64 N = 3;
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

    {
        // NOTE: This is the Corner case example in db.

        tests[2].left = {
            dbms::Column { std::vector<u64>{28, 32, 250, 256, 294, 326, 357, 381, 391, 895} },
            dbms::Column { std::vector<u64>{174, 403, 855, 754, 613, 46, 224, 980, 943, 388} },
            dbms::Column { std::vector<u64>{981, 712, 339, 431, 428, 207, 655, 409, 102, 526} },
        };
        tests[2].leftColNames.colNames = { "A", "B" };
        tests[2].leftColNames.valueColNames = { "value" };

        tests[2].right = {
            dbms::Column { std::vector<u64>{57, 72, 156, 252, 467, 490, 747, 760, 845, 955} },
            dbms::Column { std::vector<u64>{ 72, 565, 481, 457, 360, 68, 762, 155, 884, 402} },
        };
        tests[2].rightColNames.colNames = { "D" };
        tests[2].rightColNames.valueColNames = { "value" };

        tests[2].expected = dbms::JoinResult {
            dbms::ColumnGroup {
                dbms::Column {
                    std::vector<u64> { 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32,
                                       250, 250, 250, 250, 250, 250, 250, 250, 250, 250, 256, 256, 256, 256, 256, 256,
                                       256, 256, 256, 256, 294, 294, 294, 294, 294, 294, 294, 294, 294, 294, 326, 326,
                                       326, 326, 326, 326, 326, 326, 326, 326, 357, 357, 357, 357, 357, 357, 357, 357,
                                       357, 357, 381, 381, 381, 381, 381, 381, 381, 381, 381, 381, 391, 391, 391, 391,
                                       391, 391, 391, 391, 391, 391, 895, 895, 895, 895, 895, 895, 895, 895, 895, 895 }
                },
                dbms::Column {
                    std::vector<u64> { 174, 174, 174, 174, 174, 174, 174, 174, 174, 174, 403, 403, 403, 403, 403, 403,
                                       403, 403, 403, 403, 855, 855, 855, 855, 855, 855, 855, 855, 855, 855, 754, 754,
                                       754, 754, 754, 754, 754, 754, 754, 754, 613, 613, 613, 613, 613, 613, 613, 613,
                                       613, 613, 46, 46, 46, 46, 46, 46, 46, 46, 46, 46, 224, 224, 224, 224, 224, 224,
                                       224, 224, 224, 224, 980, 980, 980, 980, 980, 980, 980, 980, 980, 980, 943, 943,
                                       943, 943, 943, 943, 943, 943, 943, 943, 388, 388, 388, 388, 388, 388, 388, 388,
                                       388, 388 }
                },
                dbms::Column {
                    std::vector<u64> { 57, 72, 156, 252, 467, 490, 747, 760, 845, 955, 57, 72, 156, 252, 467, 490, 747,
                                       760, 845, 955, 57, 72, 156, 252, 467, 490, 747, 760, 845, 955, 57, 72, 156, 252,
                                       467, 490, 747, 760, 845, 955, 57, 72, 156, 252, 467, 490, 747, 760, 845, 955, 57,
                                       72, 156, 252, 467, 490, 747, 760, 845, 955, 57, 72, 156, 252, 467, 490, 747, 760,
                                       845, 955, 57, 72, 156, 252, 467, 490, 747, 760, 845, 955, 57, 72, 156, 252, 467,
                                       490, 747, 760, 845, 955, 57, 72, 156, 252, 467, 490, 747, 760, 845, 955 }
                },
                dbms::Column {
                    std::vector<u64> { 981, 981, 981, 981, 981, 981, 981, 981, 981, 981, 712, 712, 712, 712, 712, 712,
                                       712, 712, 712, 712, 339, 339, 339, 339, 339, 339, 339, 339, 339, 339, 431, 431,
                                       431, 431, 431, 431, 431, 431, 431, 431, 428, 428, 428, 428, 428, 428, 428, 428,
                                       428, 428, 207, 207, 207, 207, 207, 207, 207, 207, 207, 207, 655, 655, 655, 655,
                                       655, 655, 655, 655, 655, 655, 409, 409, 409, 409, 409, 409, 409, 409, 409, 409,
                                       102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 526, 526, 526, 526, 526, 526,
                                       526, 526, 526, 526 }
                },
                dbms::Column {
                    std::vector<u64> { 72, 565, 481, 457, 360, 68, 762, 155, 884, 402, 72, 565, 481, 457, 360, 68, 762,
                                       155, 884, 402, 72, 565, 481, 457, 360, 68, 762, 155, 884, 402, 72, 565, 481, 457,
                                       360, 68, 762, 155, 884, 402, 72, 565, 481, 457, 360, 68, 762, 155, 884, 402, 72,
                                       565, 481, 457, 360, 68, 762, 155, 884, 402, 72, 565, 481, 457, 360, 68, 762, 155,
                                       884, 402, 72, 565, 481, 457, 360, 68, 762, 155, 884, 402, 72, 565, 481, 457, 360,
                                       68, 762, 155, 884, 402, 72, 565, 481, 457, 360, 68, 762, 155, 884, 402 }
                },
            },
            dbms::ColumnNames { { "A", "B", "D" }, { "Column1", "Column2" } },
        };

        tests[2].expectedSumSquaredCol = dbms::Column {
            std::vector<u64>{ 1108809, 2390116, 2137444, 2067844, 1798281, 1100401, 3038049, 1290496, 3478225, 1912689,
                              614656, 1630729, 1423249, 1366561, 1149184, 608400, 2172676, 751689, 2547216, 1240996,
                              168921, 817216, 672400, 633616, 488601, 165649, 1212201, 244036, 1495729, 549081, 253009,
                              992016, 831744, 788544, 625681, 249001, 1423249, 343396, 1729225, 693889, 250000, 986049,
                              826281, 783225, 620944, 246016, 1416100, 339889, 1721344, 688900, 77841, 595984, 473344,
                              440896, 321489, 75625, 938961, 131044, 1190281, 370881, 528529, 1488400, 1290496, 1236544,
                              1030225, 522729, 2007889, 656100, 2368521, 1117249, 231361, 948676, 792100, 749956, 591361,
                              227529, 1371241, 318096, 1671849, 657721, 30276, 444889, 339889, 312481, 213444, 28900,
                              746496, 66049, 972196, 254016, 357604, 1190281, 1014049, 966289, 784996, 352836, 1658944,
                              463761, 1988100, 861184 }
        };
        tests[2].expectedSumSquared = 93553260;
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
    // TODO: Add a test for multiple column group joins.
    return 0;
}
