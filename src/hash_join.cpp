#include <dbms.h>

#include <unordered_map>

namespace dbms {

JoinResult hashJoin(const ColumnGroup& left,
                    const ColumnGroup& right,
                    const ColumnNames& leftColNames,
                    const ColumnNames& rightColNames) {

    auto leftToRightTranslationTable = createIndexTranslationTable(leftColNames, rightColNames);

    if (leftToRightTranslationTable.empty()) {
        logErr("No common columns found. Join is not possible.");
        return JoinResult();
    }

    const u64 leftJoinIdx = leftToRightTranslationTable[0].first;
    const u64 rightJoinIdx = leftToRightTranslationTable[0].second;

    // Build phase

    DataSource leftJoinColumnSrc = left[leftJoinIdx].data();
    std::unordered_map<u64, std::vector<u64>> hashTable;
    for (u64 i = 0; i < leftJoinColumnSrc.size(); i++) {
        hashTable[leftJoinColumnSrc[i]].push_back(i);
    }

    // Probe phase

    JoinResult result = JoinResult::createFromNames(leftColNames, rightColNames);
    DataSource rightJoinColumnSrc = right[rightJoinIdx].data();

    std::vector<std::pair<u64, u64>> columnWriteOrderForLeft;
    std::vector<std::pair<u64, u64>> columnWriteOrderForRight;
    // TODO: Make a function out of this:
    {
        std::vector<bool> used (result.names.colNames.size(), false);
        for (u64 i = 0; i < leftColNames.colNames.size(); i++) {
            const auto& leftCol = leftColNames.colNames[i];
            for (u64 j = 0; j < result.names.colNames.size(); j++) {
                if (result.names.colNames[j] == leftCol) {
                    columnWriteOrderForLeft.push_back({ j, i });
                    used[j] = true;
                    break;
                }
            }
        }

        for (u64 i = 0; i < rightColNames.colNames.size(); i++) {
            const auto& rightCol = rightColNames.colNames[i];
            for (u64 j = 0; j < result.names.colNames.size(); j++) {
                if (!used[j] && result.names.colNames[j] == rightCol) {
                    columnWriteOrderForRight.push_back({ j, i });
                    used[j] = true;
                    break;
                }
            }
        }
    }

    u64 startOfValuesInLeft = leftColNames.colNames.size();
    u64 startOfValuesInRight = rightColNames.colNames.size();
    u64 startOfValuesInResult = result.names.colNames.size();

    for (u64 rightRow = 0; rightRow < rightJoinColumnSrc.size(); rightRow++) {
        auto x = rightJoinColumnSrc[rightRow];
        auto it = hashTable.find(x);
        if (it != hashTable.end()) {
            for (auto& leftRow : it->second) {
                // Check if any of the other common columns doesn't match.
                bool match = true;
                for (u64 i = 1; i < leftToRightTranslationTable.size(); i++) {
                    auto& [leftColIdx, rightColIdx] = leftToRightTranslationTable[i];
                    auto lds = left[leftColIdx].data();
                    auto rds = right[rightColIdx].data();
                    if (lds[leftRow] != rds[rightRow]) {
                        match = false;
                        break;
                    }
                }

                if (match) {
                    // Append the matching rows to the result.
                    for (auto& [resultIdx, leftIdx] : columnWriteOrderForLeft) {
                        auto lds = left[leftIdx].data();
                        result.columns[resultIdx].append(lds[leftRow]);
                    }
                    for (auto& [resultIdx, rightIdx] : columnWriteOrderForRight) {
                        auto rds = right[rightIdx].data();
                        result.columns[resultIdx].append(rds[rightRow]);
                    }

                    u64 valueWriteIdx = startOfValuesInResult;
                    for (u64 i = startOfValuesInLeft; i < left.size(); i++) {
                        auto lds = left[i].data();
                        result.columns[valueWriteIdx].append(lds[leftRow]);
                        valueWriteIdx++;
                    }
                    for (u64 i = startOfValuesInRight; i < right.size(); i++) {
                        auto rds = right[i].data();
                        result.columns[valueWriteIdx].append(rds[rightRow]);
                        valueWriteIdx++;
                    }
                }
            }
        }
    }

    sortColumns(result.columns);

    return result;
}

} // namespace dbms
