#include "t-index.h"

i32 runAllTests() {
    std::cout << "\n" << "RUNNING TESTS" << "\n\n";

    RunTestSuite(runDBMSTestsSuite);
    RunTestSuite(runHashJoinTestsSuite);
    RunTestSuite(runCrossJoinTestsSuite);
    RunTestSuite(runMergeJoinTestsSuite);
    RunTestSuite(runBinarySearchJoinTestsSute);

    return 0;
}
