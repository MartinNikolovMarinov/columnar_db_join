#include <dbms.h>
#include "tests/t-index.h"

int main() {
    using namespace dbms;

    initSubmodules();

    i32 exitCode = runAllTests();

    return exitCode;
}
