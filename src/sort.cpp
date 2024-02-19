#include <dbms.h>

namespace dbms {

void sortDataInColumns(ColumnGroup& cols) {
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
    std::sort(indices.begin(), indices.end(), columnComparer);

    // Rearrange each column based on sorted indices
    for (auto& column : cols) {
        std::vector<u64> sortedColumn(numRows);
        // Column sortedColumn(numRows);
        for (u64 i = 0; i < numRows; i++) {
            auto cdata = column.data();
            sortedColumn[i] = cdata[indices[i]];
        }
        column = Column(std::move(sortedColumn));
    }
}

} // namespace dbms
