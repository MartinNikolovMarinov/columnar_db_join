#include <dbms.h>

namespace dbms {

JoinResult mergeJoin(const ColumnGroup& left,
                     const ColumnGroup& right,
                     const ColumnNames& leftColNames,
                     const ColumnNames& rightColNames) {

    JoinResult result = JoinResult::createFromNames(leftColNames, rightColNames);

    constexpr u64 joinIdxLeft = 0;
    constexpr u64 joinIdxRight = 0;
    u64 iLeft = 0; // row index in left table
    u64 iRight = 0; // row index in right table
    u64 nLeft = left[joinIdxLeft].data().size(); // number of rows in left table
    u64 nRight = right[joinIdxRight].data().size(); // number of rows in right table

    u64 endOfGroup = 0;

    while (iLeft < nLeft && iRight < nRight) {
        auto joinLeftDataSrc = left[joinIdxLeft].data();
        auto joinRightDataSrc = right[joinIdxRight].data();
        u64 a = joinLeftDataSrc[iLeft];
        u64 b = joinRightDataSrc[iRight];

        if (a < b) {
            iLeft++;
        }
        else if (a > b) {
            iRight = (iRight < endOfGroup) ? (endOfGroup) : (iRight + 1);
        }
        else {
            u64 k = iRight;
            while (k < nRight && joinRightDataSrc[k] == a) {
                bool matchOnSecondaryKeys = checkSecondaryKeys(leftColNames, rightColNames, left, right, iLeft, k);
                if (matchOnSecondaryKeys) {
                    appendRowToResult(result, leftColNames, rightColNames, left, right, iLeft, k);
                }
                k++;
            }

            endOfGroup = std::max(endOfGroup, k);

            iLeft++;
        }
    }

    return result;
}

} // namespace dbms
