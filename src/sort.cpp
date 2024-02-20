#include <dbms.h>

namespace dbms {

void sortDataInColumns(ColumnGroup& cols) {
    TRACE_BLOCK_CPU_TIME("sortDataInColumns");

    if (cols.empty() || cols[0].data().empty()) return;

    u64 numRows = cols[0].data().size();
    std::vector<u64> indices(numRows);

    {
        TRACE_BLOCK_CPU_TIME("sortDataInColumns - sort indices");

        bool isSorted = true;
        for (u64 i = 0; i < numRows; ++i) {
            indices[i] = i;
        }

        // Custom comparator to sort indices based on table row values
        auto columnComparer = [&cols, &isSorted](u64 a, u64 b) {
            for (const auto& column : cols) {
                auto cdata = column.data();
                if (cdata[a] != cdata[b]) {
                    isSorted = false;
                    return cdata[a] < cdata[b];
                }
            }
            return false; // Equal rows
        };

        // Sort indices based on comparator
        std::sort(indices.begin(), indices.end(), columnComparer);

        if (isSorted) return;
    }

    {
        TRACE_BLOCK_CPU_TIME("sortDataInColumns - rearrange columns");

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
}

} // namespace dbms
