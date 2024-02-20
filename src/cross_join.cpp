#include <dbms.h>

#include <thread>

namespace dbms {

namespace {

static constexpr u64 LARGE_DATASET_THRESHOLD = 500;

void crossJoinImpl(JoinResult& result,
                   const ColumnGroup& left, const ColumnGroup& right,
                   const ColumnNames& leftColNames, const ColumnNames& rightColNames,
                   u64 beginRowLeft, u64 endRowLeft,
                   u64 beginRowRight, u64 endRowRight) {

    // [BEGIN]: HOT SECTION

    for (u64 leftRow = beginRowLeft; leftRow < endRowLeft; leftRow++) {
        for (u64 rightRow = beginRowRight; rightRow < endRowRight; rightRow++) {
            dbms::appendRowToResult(result, leftColNames, rightColNames, left, right, leftRow, rightRow);
        }
    }

    // [END]: HOT SECTION
}

void crossJoinParallel(JoinResult& result,
                       const ColumnGroup& left,
                       const ColumnGroup& right,
                       const ColumnNames& leftColNames,
                       const ColumnNames& rightColNames,
                       u64 rowCountLeft, u64 rowsCountRight) {

    const u64 numThreads = std::thread::hardware_concurrency();
    std::vector<std::thread> threads;
    u64 chunkSize = rowCountLeft / numThreads;

    std::vector<JoinResult> results; // Local results for each thread
    u64 chunkSizeRemainder = rowCountLeft % numThreads;
    if (chunkSizeRemainder > 0) {
        results.resize(numThreads + 1);
    }
    else {
        results.resize(numThreads);
    }

    {
        TRACE_BLOCK_CPU_TIME("corss join parallel part");

        for (u64 i = 0; i < numThreads; ++i) {
            u64 startRow = i * chunkSize;
            u64 endRow = startRow + chunkSize;

            results[i] = JoinResult::createFromNames(leftColNames, rightColNames);

            threads.emplace_back(crossJoinImpl, std::ref(results[i]),
                                std::ref(left), std::ref(right),
                                std::ref(leftColNames), std::ref(rightColNames),
                                startRow, endRow,
                                0, rowsCountRight);
        }

        if (chunkSizeRemainder > 0) {
            u64 startRow = numThreads * chunkSize;
            u64 endRow = startRow + chunkSizeRemainder;

            results[numThreads] = JoinResult::createFromNames(leftColNames, rightColNames);

            crossJoinImpl(results[numThreads],
                        left, right,
                        leftColNames, rightColNames,
                        startRow, endRow,
                        0, rowsCountRight);
        }

        for (auto& thread : threads) {
            thread.join();
        }
    }

    {
        TRACE_BLOCK_CPU_TIME("merge local results");

        // Merge local results into the final result
        for (auto& localResult : results) {
            for (u64 i = 0; i < localResult.columns.size(); ++i) {
                result.columns[i].append(std::move(localResult.columns[i]));
            }
        }
    }
}

} // namespace

JoinResult crossJoin(const ColumnGroup& left,
                     const ColumnGroup& right,
                     const ColumnNames& leftColNames,
                     const ColumnNames& rightColNames) {

    TRACE_BLOCK_CPU_TIME("crossJoin");

    JoinResult result = JoinResult::createFromNames(leftColNames, rightColNames);

    u64 rowsCountLeft = left.empty() ? 1 : left[0].data().size();
    u64 rowsCountRight = right.empty() ? 1 : right[0].data().size();

    for (auto& col : result.columns) {
        col.reserve(rowsCountLeft * rowsCountRight);
    }

    bool isLargeDataset = std::max(rowsCountLeft, rowsCountRight) >= LARGE_DATASET_THRESHOLD;

    if (isLargeDataset) {
        crossJoinParallel(result, left, right, leftColNames, rightColNames, rowsCountLeft, rowsCountRight);
    }
    else {
        crossJoinImpl(result, left, right, leftColNames, rightColNames, 0, rowsCountLeft, 0, rowsCountRight);
    }

    // dbms::sortDataInColumns(result.columns);

    return result;
}

} // namespace dbms
