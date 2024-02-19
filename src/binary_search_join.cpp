#include <dbms.h>

namespace dbms {

namespace {

i64 binarySearch(const DataSource& dataSrc, u64 value) {
    i64 left = 0;
    i64 right = dataSrc.size() - 1;

    while (left <= right) {
        i64 mid = left + (right - left) / 2;

        if (dataSrc[mid] == value) {
            return mid;
        }

        if (dataSrc[mid] < value) {
            left = mid + 1;
        }
        else {
            right = mid - 1;
        }
    }

    return -1;
}

} // namespace

JoinResult binarySearchJoin(const ColumnGroup& left,
                            const ColumnGroup& right,
                            const ColumnNames& leftColNames,
                            const ColumnNames& rightColNames) {
    i64 joinIdxLeft = -1;
    constexpr u64 joinIdxRight = 0;

    for (u64 i = 0; i < leftColNames.colNames.size(); i++) {
        if (leftColNames.colNames[i] == rightColNames.colNames[joinIdxRight]) {
            joinIdxLeft = i;
            break;
        }
    }

    if (joinIdxLeft == -1) {
        logErr("The clustered index of the right column is not found in the left column.");
        return JoinResult();
    }

    JoinResult result = JoinResult::createFromNames(leftColNames, rightColNames);
    const auto& leftDataSrc = left[joinIdxLeft].data();
    const auto& rightDataSrc = right[joinIdxRight].data();

    for (u64 leftRow = 0; leftRow < leftDataSrc.size(); leftRow++) {
        u64 leftValue = leftDataSrc[leftRow];

        i64 rightValueIdx = binarySearch(rightDataSrc, leftValue);
        if (rightValueIdx != -1) {

            // Find the first occurrence of the value in the right table:
            while (rightValueIdx > 0 && rightDataSrc[rightValueIdx - 1] == leftValue) {
                rightValueIdx--;
            }

            // Iterate through all occurrences of the value in the right table:
            while (u64(rightValueIdx) < rightDataSrc.size() && rightDataSrc[rightValueIdx] == leftValue) {
                bool match = dbms::checkSecondaryKeys(leftColNames, rightColNames, left, right, leftRow, rightValueIdx);
                if (match) {
                    dbms::appendRowToResult(result, leftColNames, rightColNames, left, right, leftRow, rightValueIdx);
                }
                rightValueIdx++;
            }
        }
    }

    return result;
}

} // namespace dbms
