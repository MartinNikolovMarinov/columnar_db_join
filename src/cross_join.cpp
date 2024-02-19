#include <dbms.h>

namespace dbms {

JoinResult crossJoin(const ColumnGroup& left,
                     const ColumnGroup& right,
                     const ColumnNames& leftColNames,
                     const ColumnNames& rightColNames) {

    JoinResult result = JoinResult::createFromNames(leftColNames, rightColNames);

    u64 rowsCountLeft = left.empty() ? 1 : left[0].data().size();
    u64 rowsCountRight = right.empty() ? 1 : right[0].data().size();
    for (auto& col : result.columns) {
        col.reserve(rowsCountLeft * rowsCountRight);
    }

    // Fill the result table with all combinations of rows from t1 and t2
    u64 rowIdx = 0;
    for (u64 leftRow = 0; leftRow < rowsCountLeft; leftRow++) {
        for (u64 rightRow = 0; rightRow < rowsCountRight; rightRow++) {
            dbms::appendRowToResult(result, leftColNames, rightColNames, left, right, leftRow, rightRow);

            rowIdx++;
        }
    }

    return result;
}

} // namespace dbms
