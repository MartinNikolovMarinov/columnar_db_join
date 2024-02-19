#include <dbms.h>

#include <unordered_map>

namespace dbms {

namespace {

using JoinHashMap = std::unordered_map<u64, std::vector<u64>>;

JoinHashMap buildPhase(DataSource colGroupSrc) {
    JoinHashMap hashTable;

    for (u64 i = 0; i < colGroupSrc.size(); i++) {
        hashTable[colGroupSrc[i]].push_back(i);
    }

    return hashTable;
}

JoinResult probePhase(const ColumnGroup& left,
                      const ColumnGroup& right,
                      const ColumnNames& leftColNames,
                      const ColumnNames& rightColNames,
                      const JoinHashMap& hashTable,
                      const IndexTranslationTable& leftToRightTranslationTable) {

    const u64 rightJoinIdx = leftToRightTranslationTable[0].second;

    JoinResult result = JoinResult::createFromNames(leftColNames, rightColNames);
    DataSource rightJoinColumnSrc = right[rightJoinIdx].data();

    const auto& writeOrder = createTableWriteOrder(leftColNames, rightColNames, result.names);

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
                    for (auto& [resultIdx, leftIdx] : writeOrder.first) {
                        auto lds = left[leftIdx].data();
                        result.columns[resultIdx].append(lds[leftRow]);
                    }
                    for (auto& [resultIdx, rightIdx] : writeOrder.second) {
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

    return result;
}

} // namespace


JoinResult hashJoin(const ColumnGroup& left,
                    const ColumnGroup& right,
                    const ColumnNames& leftColNames,
                    const ColumnNames& rightColNames) {

    auto leftToRightTranslationTable = createIndexTranslationTable(leftColNames, rightColNames);

    if (leftToRightTranslationTable.empty()) {
        logErr("No common columns found. Hash join is not possible.");
        return JoinResult();
    }
    if (left.empty() || right.empty()) {
        logErr("One of the tables is empty. Hash join is not possible.");
        return JoinResult();
    }

    // Build phase
    const u64 leftJoinIdx = leftToRightTranslationTable[0].first;
    auto joinColumnDataSrc = left[leftJoinIdx].data();
    auto hashTable = buildPhase(joinColumnDataSrc);

    // Probe phase
    auto result = probePhase(left, right, leftColNames, rightColNames, hashTable, leftToRightTranslationTable);

    return result;
}

} // namespace dbms
