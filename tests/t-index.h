#pragma once

#include <dbms.h>

#include <iostream>

inline i32 g_testCount = 0;

#define RunTest(test, ...) \
    g_testCount++; \
    std::cout << "\t[TEST " << "№ " << g_testCount << " RUNNING] " << ANSI_BOLD(#test) << '\n'; \
    { [[maybe_unused]] auto __notused__ = test(__VA_ARGS__); } \
    std::cout << "\t[TEST " << "№ " << g_testCount << ANSI_BOLD(ANSI_GREEN(" PASSED")) << "] " << ANSI_BOLD(#test) << std::endl;

#define RunTestSuite(suite, ...) \
    std::cout << "[SUITE RUNNING] " << ANSI_BOLD(#suite) << std::endl; \
    suite(__VA_ARGS__); \
    std::cout << "[SUITE " << ANSI_BOLD(ANSI_GREEN("PASSED")) << "] " << ANSI_BOLD(#suite) << std::endl;

i32 runDBMSTestsSuite();
i32 runHashJoinTestsSuite();

i32 runAllTests();
