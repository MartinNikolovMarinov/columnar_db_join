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

    const auto& writeOrder = createTableWriteOrder(leftColNames, rightColNames, result.names);

    u64 startOfValuesInLeft = leftColNames.colNames.size();
    u64 startOfValuesInRight = rightColNames.colNames.size();
    u64 startOfValuesInResult = result.names.colNames.size();

    // Fill the result table with all combinations of rows from t1 and t2
    u64 rowIdx = 0;
    for (u64 i = 0; i < rowsCountLeft; i++) {
        for (u64 j = 0; j < rowsCountRight; j++) {
            for (auto& [resultIdx, leftIdx] : writeOrder.first) {
                auto lds = left[leftIdx].data();
                result.columns[resultIdx].append(lds[i]);
            }
            for (auto& [resultIdx, rightIdx] : writeOrder.second) {
                auto rds = right[rightIdx].data();
                result.columns[resultIdx].append(rds[j]);
            }

            u64 valueWriteIdx = startOfValuesInResult;
            for (u64 k = startOfValuesInLeft; k < left.size(); k++) {
                auto ldata = left[k].data();
                result.columns[valueWriteIdx].append(ldata[i]);
                valueWriteIdx++;
            }
            for (u64 k = startOfValuesInRight; k < right.size(); k++) {
                auto rdata = right[k].data();
                result.columns[valueWriteIdx].append(rdata[j]);
                valueWriteIdx++;
            }

            rowIdx++;
        }
    }

    return result;
}

} // namespace dbms
