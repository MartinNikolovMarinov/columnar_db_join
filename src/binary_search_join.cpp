#include <dbms.h>

namespace dbms {

JoinResult binarySearchJoin(const ColumnGroup& left,
                            const ColumnGroup& right,
                            const ColumnNames& leftColNames,
                            const ColumnNames& rightColNames) {

    JoinResult result = JoinResult::createFromNames(leftColNames, rightColNames);

    return result;
}

} // namespace dbms
