#include <dbms.h>
#include "tests/t-index.h"

i32 main() {
    using namespace dbms;

    initSubmodules(core::LogLevel::L_WARNING);

    i32 exitCode = runAllTests();

    return exitCode;
}
