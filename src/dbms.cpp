#include <dbms.h>

namespace dbms {

void initSubmodules() {
    core::initLoggingSystem(core::LogLevel::L_TRACE, nullptr, 0);

    logInfo("DBMS_DEBUG = " DBMS_DEBUG);
    logInfo("DBMS_VERSION = " DBMS_VERSION);
    logInfo("DBMS_BINARY_PATH = " DBMS_BINARY_PATH);
    logInfo("DBMS_DATA_PATH = " DBMS_DATA_PATH);
}

} // namespace dbms

