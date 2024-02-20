#include <dbms.h>

#include <thread>

namespace dbms {

namespace {

static constexpr u64 LARGE_DATASET_THRESHOLD = 10000;

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

void searchAndJoin(JoinResult& result,
                   const ColumnGroup& left, DataSource leftDataSrc,
                   const ColumnGroup& right, DataSource rightDataSrc,
                   const ColumnNames& leftColNames, const ColumnNames& rightColNames,
                   u64 beginRow, u64 endRow)  {
    // [BEGIN]: HOT SECTION

    for (u64 leftRow = beginRow; leftRow < endRow; leftRow++) {
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

    // [END]: HOT SECTION
}

void searchAndJoinParallel(JoinResult& result,
                           const ColumnGroup& left, DataSource leftDataSrc,
                           const ColumnGroup& right, DataSource rightDataSrc,
                           const ColumnNames& leftColNames, const ColumnNames& rightColNames)  {

    const u64 numThreads = std::thread::hardware_concurrency();
    std::vector<std::thread> threads;
    u64 chunkSize = leftDataSrc.size() / numThreads;

    u64 chunkSizeRemainder = leftDataSrc.size() % numThreads;
    std::vector<JoinResult> results; // Local results for each thread
    if (chunkSizeRemainder > 0) {
        results.resize(numThreads + 1);
    }
    else {
        results.resize(numThreads);
    }

    for (u64 i = 0; i < numThreads; ++i) {
        u64 startRow = i * chunkSize;
        u64 endRow = startRow + chunkSize;

        results[i] = JoinResult::createFromNames(leftColNames, rightColNames);

        threads.emplace_back(searchAndJoin, std::ref(results[i]),
                             std::ref(left), std::ref(leftDataSrc),
                             std::ref(right), std::ref(rightDataSrc),
                             std::ref(leftColNames), std::ref(rightColNames),
                             startRow, endRow);
    }

    if (chunkSizeRemainder > 0) {
        u64 startRow = numThreads * chunkSize;
        u64 endRow = startRow + chunkSizeRemainder;

        results[numThreads] = JoinResult::createFromNames(leftColNames, rightColNames);

        searchAndJoin(results[numThreads],
                      left, leftDataSrc,
                      right, rightDataSrc,
                      leftColNames, rightColNames,
                      startRow, endRow);
    }

    for (auto& thread : threads) {
        thread.join();
    }

    // Merge local results into the final result
    for (auto& localResult : results) {
        for (u64 i = 0; i < localResult.columns.size(); ++i) {
            result.columns[i].append(std::move(localResult.columns[i]));
        }
    }
}

} // namespace

JoinResult binarySearchJoin(const ColumnGroup& left,
                            const ColumnGroup& right,
                            const ColumnNames& leftColNames,
                            const ColumnNames& rightColNames) {
    TRACE_BLOCK_CPU_TIME("binarySearchJoin");

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

    u64 rowCount = leftDataSrc.size();
    bool isLargeDataset = rowCount >= LARGE_DATASET_THRESHOLD;

    if (isLargeDataset) {
        searchAndJoinParallel(result, left, leftDataSrc, right, rightDataSrc, leftColNames, rightColNames);
        dbms::sortDataInColumns(result.columns);
    }
    else {
        searchAndJoin(result, left, leftDataSrc, right, rightDataSrc, leftColNames, rightColNames, 0, rowCount);
        dbms::sortDataInColumns(result.columns);
    }

    return result;
}

} // namespace dbms
