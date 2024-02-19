#include <dbms.h>

#include <optional>

namespace dbms {

// TODO: Move this to dbms and re-use it in other joins.
void appendRowToResult(JoinResult& result,
                       const ColumnNames& leftColNames,
                       const ColumnNames& rightColNames,
                       const ColumnGroup& left,
                       const ColumnGroup& right,
                       u64 leftRowIdx, u64 rightRowIdx) {

    u64 startOfValuesInLeft = leftColNames.colNames.size();
    u64 startOfValuesInRight = rightColNames.colNames.size();
    u64 startOfValuesInResult = result.names.colNames.size();

    const auto& writeOrder = createTableWriteOrder(leftColNames, rightColNames, result.names);

    // Append the matching rows to the result.
    for (auto& [resultIdx, leftIdx] : writeOrder.first) {
        auto lds = left[leftIdx].data();
        result.columns[resultIdx].append(lds[leftRowIdx]);
    }
    for (auto& [resultIdx, rightIdx] : writeOrder.second) {
        auto rds = right[rightIdx].data();
        result.columns[resultIdx].append(rds[rightRowIdx]);
    }

    u64 valueWriteIdx = startOfValuesInResult;
    for (u64 i = startOfValuesInLeft; i < left.size(); i++) {
        auto lds = left[i].data();
        result.columns[valueWriteIdx].append(lds[leftRowIdx]);
        valueWriteIdx++;
    }
    for (u64 i = startOfValuesInRight; i < right.size(); i++) {
        auto rds = right[i].data();
        result.columns[valueWriteIdx].append(rds[rightRowIdx]);
        valueWriteIdx++;
    }
}

bool checkSecondaryKeys(const ColumnNames& leftColNames,
                        const ColumnNames& rightColNames,
                        const ColumnGroup& left,
                        const ColumnGroup& right,
                        u64 leftRowIdx, u64 rightRowIdx) {

    bool match = true;
    const auto& leftToRightTranslationTable = createIndexTranslationTable(leftColNames, rightColNames);

    // Check if all of the secondary indices match.
    for (u64 i = 1; i < leftToRightTranslationTable.size(); i++) {
        const auto& [leftColIdx, rightColIdx] = leftToRightTranslationTable[i];
        auto ldataCurr = left[leftColIdx].data();
        auto rdataCurr = right[rightColIdx].data();
        if (ldataCurr[leftRowIdx] != rdataCurr[rightRowIdx]) {
            match = false;
            break;
        }
    }

    return match;
}

JoinResult mergeJoin(const ColumnGroup& left,
                     const ColumnGroup& right,
                     const ColumnNames& leftColNames,
                     const ColumnNames& rightColNames) {

    JoinResult result = JoinResult::createFromNames(leftColNames, rightColNames);
    auto leftToRightTranslationTable = createIndexTranslationTable(leftColNames, rightColNames);

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
