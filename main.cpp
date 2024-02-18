// #include <dbms.h>

// struct ColumnData {
//     const u64* src;
// };

// struct Table {
//     u64 columnDataCount;
//     std::vector<std::string> columnNames;
//     std::vector<ColumnData> columnData;

//     bool isValid() {
//         if (columnData.size() < 2 ||
//             columnData.size() != columnNames.size()) {
//             logWarn("Column sizes in table do not match");
//             return false;
//         }

//         for (u64 i = 0; i < columnNames.size(); i++) {
//             if ((i < columnNames.size() - 2) && columnNames[i] > columnNames[i + 1]) {
//                 // The columns need to be lexically ordered. Except for the last one.
//                 logWarn("Column names are not lexically ordered");
//                 return false;
//             }

//             if (columnData[i].src == nullptr) {
//                 // The column data must not be a nullptr.
//                 logWarn("Column data is nullptr");
//                 return false;
//             }
//         }

//         return true;
//     }
// };

// struct AlignedTable {
//     std::vector<std::string> columnNames;
//     std::vector<std::vector<u64>> columnData;
//     std::vector<std::vector<u64>> valueColumns;

//     AlignedTable() : m_cap(0), m_writeIdx(0) {}

//     inline u64 cap() const { return m_cap; }

//     inline void advanceWriteIdx() {
//         m_writeIdx++;
//         if (m_writeIdx > m_cap) {
//             m_cap = m_writeIdx;
//         }
//     }

//     inline void writeAt(u64 columnIdx, u64 value) {
//         auto& colData = columnData[columnIdx];
//         if (m_writeIdx >= colData.size()) {
//             colData.push_back(value);
//         }
//         else {
//             colData[m_writeIdx] = value;
//         }
//     }

//     inline void reset() {
//         m_writeIdx = 0;
//     }

// private:
//     u64 m_cap;
//     u64 m_writeIdx;
// };

// void debug_printAlignedTable(AlignedTable& t) {
//     std::string result;

//     for (u64 i = 0; i < t.columnNames.size(); i++) {
//         result += t.columnNames[i] + ' ';
//     }

//     for (u64 i = 0; i < t.cap(); i++) {
//         result += "\n";
//         for (u64 j = 0; j < t.columnData.size(); j++) {
//             result += std::to_string(t.columnData[j][i]) + ' ';
//         }
//     }

//     logInfo("\n%s", result.data());
// }

// AlignedTable createAlignedTable(const std::vector<Table>& tables) {
//     TRACE_BLOCK_CPU_TIME("createAlignedTable CPU time");

//     if (tables.empty()) {
//         return AlignedTable();
//     }

//     AlignedTable result;

//     std::vector<std::string> uniqueColNames;

//     for (auto& table : tables) {
//         for (auto& colName : table.columnNames) {
//             if (colName != "value") {
//                 core::push_back_unique(uniqueColNames, colName);
//             }
//         }
//     }

//     std::sort(uniqueColNames.begin(), uniqueColNames.end());
//     result.columnData.resize(uniqueColNames.size());
//     result.columnNames = std::move(uniqueColNames);
//     result.valueColumns.resize(tables.size());

//     return result;
// }

// /**
//  * @brief TODO: Write description
//  *
//  * Assumes that the tables are valid!
// */
// void alignOn2ClusteredIdx(AlignedTable& result, const Table& t1, const Table& t2) {
//     auto t1src = t1.columnData[0].src;
//     auto t2src = t2.columnData[0].src;
//     u64 t1count = t1.columnDataCount;
//     u64 t2count = t2.columnDataCount;
//     u64 t1curr = 0;
//     u64 t2curr = 0;
//     std::optional<u64> t2FirstMatchInGroup;

//     auto ttableFromT1toT2 = dbms::createIndexTranslationTable(t1.columnNames, t1.columnNames.size() - 1, t2.columnNames, t2.columnNames.size() - 1);
//     auto ttableFromResultToT1 = dbms::createIndexTranslationTable(result.columnNames, result.columnNames.size(), t1.columnNames, t1.columnNames.size() - 1);
//     auto ttableFromResultToT2 = dbms::createIndexTranslationTable(result.columnNames, result.columnNames.size(), t2.columnNames, t2.columnNames.size() - 1);

//     while (t1curr < t1count && t2curr < t2count) {
//         auto a = *(t1src + t1curr);
//         auto b = *(t2src + t2curr);

//         bool match = a == b; // clustered indices match.

//         if (match) {
//             // Check if all of the secondary indices match.
//             for (auto& [t1idx, t2idx] : ttableFromT1toT2) {
//                 if (t1.columnData[t1idx].src[t1curr] != t2.columnData[t2idx].src[t2curr]) {
//                     match = false;
//                     break;
//                 }
//             }

//             if (match) {
//                 // Write from the first table:
//                 for (auto& [resultIdx, t1idx] : ttableFromResultToT1) {
//                     u64 value = t1.columnData[t1idx].src[t1curr];
//                     result.writeAt(resultIdx, value);
//                 }

//                 // Write from the second table:
//                 for (auto& [resultIdx, t2idx] : ttableFromResultToT2) {
//                     u64 value = t2.columnData[t2idx].src[t2curr];
//                     result.writeAt(resultIdx, value);
//                 }

//                 result.advanceWriteIdx();
//             }

//             if (!t2FirstMatchInGroup.has_value()) {
//                 t2FirstMatchInGroup = t2curr;
//             }
//             t2curr++;
//         }
//         else if (a < b) {
//             if (t2FirstMatchInGroup.has_value()) {
//                 t2curr = t2FirstMatchInGroup.value();
//                 t2FirstMatchInGroup.reset();
//             }
//             t1curr++;
//         }
//         else {
//             if (t2FirstMatchInGroup.has_value()) {
//                 t2curr = t2FirstMatchInGroup.value();
//                 t2FirstMatchInGroup.reset();
//             }
//             t2curr++;
//         }
//     }
// }


// int main() {
//     dbms::initSubmodules();

//     Table t1;
//     u64 t1c1data[] = { 42, 42, 42, 42, 85, 86, 86 };
//     u64 t1c2data[] = { 0, 1, 7, 8, 1, 1, 2 };
//     u64 t1valdata[] = { 1, 3, 5, 5, 3, 7, 0 };
//     {
//         t1.columnNames = {"A", "B", "value"};

//         constexpr addr_size c1dataSize = sizeof(t1c1data) / sizeof(t1c1data[0]);
//         constexpr addr_size c2dataSize = sizeof(t1c2data) / sizeof(t1c2data[0]);
//         constexpr addr_size valdataSize = sizeof(t1valdata) / sizeof(t1valdata[0]);

//         // Sanity check:
//         static_assert(c1dataSize == c2dataSize);
//         static_assert(c1dataSize == valdataSize);

//         t1.columnDataCount = c1dataSize;
//         t1.columnData.push_back({ t1c1data  });
//         t1.columnData.push_back({ t1c2data  });
//         t1.columnData.push_back({ t1valdata });
//     }

//     Table t2;
//     u64 t2c1data[] = { 42, 42, 86, 86, 86, 86, 86, 92 };
//     u64 t2c2data[] = { 9, 10, 2, 6, 9, 9, 32, 7};
//     u64 t2c3data[] = { 7, 8, 9, 10, 11, 17, 12, 13 };
//     u64 t2valdata[] = { 4, 5, 17, 3, 19, 7, 5, 5 };
//     {
//         t2.columnNames = {"A", "C", "D", "value"};

//         constexpr addr_size c1dataSize = sizeof(t2c1data) / sizeof(t2c1data[0]);
//         constexpr addr_size c2dataSize = sizeof(t2c2data) / sizeof(t2c2data[0]);
//         constexpr addr_size c3dataSize = sizeof(t2c3data) / sizeof(t2c3data[0]);
//         constexpr addr_size valdataSize = sizeof(t2valdata) / sizeof(t2valdata[0]);

//         // Sanity check:
//         static_assert(c1dataSize == c2dataSize);
//         static_assert(c1dataSize == c3dataSize);
//         static_assert(c1dataSize == valdataSize);

//         t2.columnDataCount = c1dataSize;
//         t2.columnData.push_back({ t2c1data  });
//         t2.columnData.push_back({ t2c2data  });
//         t2.columnData.push_back({ t2c3data  });
//         t2.columnData.push_back({ t2valdata });
//     }

//     Assert(t1.isValid());
//     Assert(t2.isValid());

//     std::vector<Table> tables = { std::move(t1), std::move(t2) };
//     AlignedTable tresult = createAlignedTable(tables);
//     alignOn2ClusteredIdx(tresult, tables[0], tables[1]);
//     debug_printAlignedTable(tresult);
// }

#include <dbms.h>

#include <unordered_map>
#include <optional>
#include <vector>

struct DataSource {

    DataSource() : m_size(0), m_src(nullptr) {}
    DataSource(u64 size, const u64* src) : m_size(size), m_src(src) {}

    inline u64 operator[](u64 idx) const {
        Assert(idx < m_size, "Index out of bounds");
        return m_src[idx];
    }

    inline u64 size() const {
        return m_size;
    }

    inline bool empty() const {
        return m_size == 0;
    }

private:
    u64 m_size;
    const u64* m_src;
};

struct Column {

    Column() : m_size(0), m_mappedFile(nullptr) {}
    Column(u64 size, const u64* mappedFile) : m_size(size), m_mappedFile(mappedFile) {}
    Column(u64 size, std::vector<u64>&& inMemoryData) : m_size(size), m_inMemoryData(std::move(inMemoryData)) {}

    inline DataSource data() const {
        if (m_mappedFile != nullptr) {
            return DataSource(m_size, m_mappedFile);
        }
        return DataSource(m_inMemoryData.size(), m_inMemoryData.data());
    }

    inline void append(u64 x) {
        Assert(m_mappedFile == nullptr, "Cannot append to a column that is backed by a file");
        m_inMemoryData.push_back(x);
    }

private:
    u64 m_size;
    const u64* m_mappedFile;
    std::vector<u64> m_inMemoryData;
};

struct ColumnNames {
    std::vector<std::string> colNames;
    std::vector<std::string> valueColNames;
};

std::vector<std::pair<u64, u64>> createIndexTranslationTable(const ColumnNames& from, const ColumnNames& to) {
    std::vector<std::pair<u64, u64>> ttable;
    for (u64 i = 0; i < from.colNames.size(); i++) {
        for (u64 j = 0; j < to.colNames.size(); j++) {
            if (from.colNames[i] == to.colNames[j]) {
                ttable.push_back({ i, j });
                break;
            }
        }
    }
    return ttable;
}

using ColumnGroup = std::vector<Column>;

inline u64 readValueAt(ColumnGroup& columns, u64 columnIdx, u64 rowIdx) {
    return columns[columnIdx].data()[rowIdx];
}

using IndexTranslationTable = std::vector<std::pair<u64, u64>>;

struct JoinResult {
    ColumnGroup columns;
    ColumnNames names;
};

JoinResult createFromNames(const ColumnNames& a, const ColumnNames& b) {
    JoinResult result;

    for (u64 i = 0; i < a.colNames.size(); i++) {
        core::push_back_unique(result.names.colNames, a.colNames[i]);
    }

    for (u64 i = 0; i < b.colNames.size(); i++) {
        core::push_back_unique(result.names.colNames, b.colNames[i]);
    }

    std::sort(result.names.colNames.begin(), result.names.colNames.end());

    for (u64 i = 0; i < a.valueColNames.size(); i++) {
        result.names.valueColNames.emplace_back("Column" + std::to_string(a.valueColNames.size()));
    }

    for (u64 i = 0; i < b.valueColNames.size(); i++) {
        result.names.valueColNames.emplace_back("Column" + std::to_string(a.valueColNames.size()));
    }

    result.columns.resize(result.names.colNames.size() + result.names.valueColNames.size());
    return result;
}

void sortColumns(ColumnGroup& cols) {
    if (cols.empty() || cols[0].data().empty()) return;

    u64 numRows = cols[0].data().size();
    std::vector<u64> indices(numRows);
    for (u64 i = 0; i < numRows; ++i) {
        indices[i] = i;
    }

    // Custom comparator to sort indices based on table row values
    auto columnComparer = [&cols](u64 a, u64 b) {
        for (const auto& column : cols) {
            auto cdata = column.data();
            if (cdata[a] != cdata[b]) return cdata[a] < cdata[b];
        }
        return false; // Equal rows
    };

    // Sort indices based on comparator
    sort(indices.begin(), indices.end(), columnComparer);

    // Rearrange each column based on sorted indices
    for (auto& column : cols) {
        std::vector<u64> sortedColumn(numRows);
        // Column sortedColumn(numRows);
        for (u64 i = 0; i < numRows; i++) {
            auto cdata = column.data();
            sortedColumn[i] = cdata[indices[i]];
        }
        column = Column(numRows, std::move(sortedColumn));
    }
}

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

    JoinResult result = createFromNames(leftColNames, rightColNames);
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

int main() {
    dbms::initSubmodules();

    constexpr u64 N = 3;
    std::vector<ColumnGroup> columns (N);
    std::vector<ColumnNames> columnNames (N);

    {
        columns[0].emplace_back(5, std::vector<u64>{199, 29, 399, 499, 59});
        // columns[0].emplace_back(5, std::vector<u64>{2, 1, 2, 1, 2});
        columns[0].emplace_back(5, std::vector<u64>{1, 1, 1, 1, 2});
        columns[0].emplace_back(5, std::vector<u64>{11, 12, 13, 14, 15});
        columns[0].emplace_back(5, std::vector<u64>{16, 17, 18, 19, 20});
        columnNames[0].colNames = { "A", "C", "F" };
        columnNames[0].valueColNames = { "value" };
    }
    {
        columns[1].emplace_back(7, std::vector<u64>{100, 200, 300, 400, 500, 600, 700});
        // columns[1].emplace_back(7, std::vector<u64>{1, 2, 2, 3, 2, 1, 2});
        columns[1].emplace_back(7, std::vector<u64>{1, 2, 21, 13, 42, 13, 2});
        columns[1].emplace_back(7, std::vector<u64>{99, 12, 13, 14, 15, 16, 17});
        columns[1].emplace_back(7, std::vector<u64>{16, 17, 18, 19, 20, 21, 22});
        columnNames[1].colNames = { "B", "C", "D" };
        columnNames[1].valueColNames = { "value" };
    }
    {
        columns[2].emplace_back(4, std::vector<u64>{16, 266, 366, 466});
        columns[2].emplace_back(4, std::vector<u64>{6, 7, 8, 9});
        columns[2].emplace_back(4, std::vector<u64>{16, 17, 18, 19});
        columnNames[2].colNames = { "E", "F" };
        columnNames[2].valueColNames = { "value" };
    }

    JoinResult result = hashJoin(columns[0], columns[1], columnNames[0], columnNames[1]);
    // result = hashJoin(result.columns, columns[2], result.columnNames, columnNames[2]);

    return 0;
}
