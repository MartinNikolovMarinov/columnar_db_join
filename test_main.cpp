#include <dbms.h>
#include "tests/t-index.h"

int main() {
    using namespace dbms;

    initSubmodules(core::LogLevel::L_WARNING);

    i32 exitCode = runAllTests();

    return exitCode;
}
