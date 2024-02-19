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

    for (u64 rightRow = 0; rightRow < rightJoinColumnSrc.size(); rightRow++) {
        auto x = rightJoinColumnSrc[rightRow];
        auto it = hashTable.find(x);
        if (it != hashTable.end()) {
            for (auto& leftRow : it->second) {

                bool match = dbms::checkSecondaryKeys(leftColNames, rightColNames, left, right, leftRow, rightRow);

                if (match) {
                    dbms::appendRowToResult(result, leftColNames, rightColNames, left, right, leftRow, rightRow);
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
