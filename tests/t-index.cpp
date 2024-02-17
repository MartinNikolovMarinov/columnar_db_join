#include "t-index.h"

#include <iostream>

inline i32 g_testCount = 0;

#define RunTest(test, ...) \
    g_testCount++; \
    std::cout << "[TEST " << "№ " << g_testCount << " RUNNING] " << ANSI_BOLD(#test) << '\n'; \
    { [[maybe_unused]] auto __notused__ = test(__VA_ARGS__); } \
    std::cout << "[TEST " << "№ " << g_testCount << ANSI_BOLD(ANSI_GREEN(" PASSED")) << "] " << ANSI_BOLD(#test) << std::endl;

i32 testIndexTranslationTable() {
    struct TestCase {
        std::vector<std::string> from;
        u64 fromCount;
        std::vector<std::string> to;
        u64 toCount;
        std::vector<std::pair<u64, u64>> expected;
    };

    TestCase tests[] = {
        { { "A", "C", "E" }, 3, { "A", "B", "C", "E" }, 4, { {0, 0}, {1, 2}, {2, 3} } },
        { { "A", "B", "C" }, 3, { "A", "B" }, 3, { {0, 0}, {1, 1} } },
        { { "A", "B", "C" }, 3, { }, 0, { } },
        { { }, 0, { "A", "B", "C" }, 3, { } },
        { { "A", "B", "C" }, 3, { "A", "B", "C" }, 3, { {0, 0}, {1, 1}, {2, 2} } },
        { { "B", "D", "F" }, 3, { "A", "B", "C", "D" }, 3, { {0, 1} } },
    };

    for (auto& t : tests) {
        auto result = dbms::createIndexTranslationTable(t.from, t.fromCount, t.to, t.toCount);
        Assert(result == t.expected, "Index translation table is not as expected.");
    }

    return 0;
}

i32 runAllTests() {
    RunTest(testIndexTranslationTable);
    return 0;
}
