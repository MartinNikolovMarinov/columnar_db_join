#include <dbms.h>

#include <optional>

namespace dbms {

JoinResult mergeJoin(const ColumnGroup& left,
                     const ColumnGroup& right,
                     const ColumnNames& leftColNames,
                     const ColumnNames& rightColNames) {

    JoinResult result = JoinResult::createFromNames(leftColNames, rightColNames);
    const auto& writeOrder = createTableWriteOrder(leftColNames, rightColNames, result.names);
    auto leftToRightTranslationTable = createIndexTranslationTable(leftColNames, rightColNames);

    constexpr u64 joinIdxLeft = 0;
    constexpr u64 joinIdxRight = 0;
    u64 iLeft = 0; // row index in left table
    u64 iRight = 0; // row index in right table
    u64 nLeft = left[0].data().size(); // number of rows in left table
    u64 nRight = right[0].data().size(); // number of rows in right table
    std::optional<u64> firstMatchInGroup;

    u64 startOfValuesInLeft = leftColNames.colNames.size();
    u64 startOfValuesInRight = rightColNames.colNames.size();
    u64 startOfValuesInResult = result.names.colNames.size();

    while (iLeft < nLeft && iRight < nRight) {
        auto ldata = left[joinIdxLeft].data();
        auto rdata = right[joinIdxRight].data();
        u64 a = ldata[iLeft];
        u64 b = rdata[iRight];

        if (a < b) {
            if (firstMatchInGroup.has_value()) {
                iRight = firstMatchInGroup.value();
                firstMatchInGroup.reset();
            }
            iLeft++;
        }
        else if (a > b) {
            if (firstMatchInGroup.has_value()) {
                iRight = firstMatchInGroup.value();
                firstMatchInGroup.reset();
            }
            iRight++;
        }
        else {
            bool match = true;

            // Check if all of the secondary indices match.
            for (auto& [leftColIdx, rightColIdx] : leftToRightTranslationTable) {
                auto ldataCurr = left[leftColIdx].data();
                auto rdataCurr = right[rightColIdx].data();
                if (ldataCurr[iLeft] != rdataCurr[iRight]) {
                    match = false;
                    break;
                }
            }

            if (match) {
                // Append the matching rows to the result.
                for (auto& [resultIdx, leftIdx] : writeOrder.first) {
                    auto lds = left[leftIdx].data();
                    result.columns[resultIdx].append(lds[iLeft]);
                }
                for (auto& [resultIdx, rightIdx] : writeOrder.second) {
                    auto rds = right[rightIdx].data();
                    result.columns[resultIdx].append(rds[iRight]);
                }

                u64 valueWriteIdx = startOfValuesInResult;
                for (u64 i = startOfValuesInLeft; i < left.size(); i++) {
                    auto lds = left[i].data();
                    result.columns[valueWriteIdx].append(lds[iLeft]);
                    valueWriteIdx++;
                }
                for (u64 i = startOfValuesInRight; i < right.size(); i++) {
                    auto rds = right[i].data();
                    result.columns[valueWriteIdx].append(rds[iRight]);
                    valueWriteIdx++;
                }
            }

            if (!firstMatchInGroup.has_value()) {
                firstMatchInGroup = iRight;
            }
            iRight++;
        }
    }

    return result;
}

} // namespace dbms
