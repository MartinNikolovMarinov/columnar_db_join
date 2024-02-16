// #include <sys/mman.h>
// #include <sys/types.h>
// #include <sys/stat.h>
// #include <unistd.h>
// #include <fcntl.h>
// #include <errno.h>
// #include <string.h>
// #include <stdio.h>

// #include <mio/mio.hpp>

// #include <vector>
// #include <unordered_set>

// int handleError(const std::error_code& error)
// {
//     const auto& errmsg = error.message();
//     std::printf("error mapping file: %s, exiting...\n", errmsg.c_str());
//     return error.value();
// }

// bool binarySearch(const size_t* arr, size_t size, size_t n) {
//     if (!arr || size == 0) {
//         return false; // Handle null pointer or empty array
//     }

//     size_t left = 0;
//     size_t right = size - 1;

//     while (left <= right) {
//         size_t mid = left + (right - left) / 2; // Avoid overflow

//         if (arr[mid] == n) {
//             return true;
//         } else if (arr[mid] < n) {
//             left = mid + 1;
//         } else {
//             // To prevent underflow, check if mid is 0
//             if (mid == 0) break; // n is less than the first element
//             right = mid - 1;
//         }
//     }

//     return false;
// }

// int repetitionCount = 20;
// const char* path = PATH_TO_DB"/my-example/sample.txt";
// // const char* path = PATH_TO_DB"/small-example/column1/key_A.bin";

// int main() {
//     std::error_code error;
//     mio::mmap_source mmap = mio::make_mmap_source(path, 0, mio::map_entire_file, error);

//     if (error) {
//         return handleError(error);
//     }

//     printf("Starting work on dataset with size: %lu\n", mmap.size());

//     const size_t sss = mmap.size() / sizeof(size_t) + 0.20 * (mmap.size() / sizeof(size_t));
//     std::vector<size_t> v1;
//     std::vector<size_t> v2;
//     std::vector<size_t> v3;
//     std::vector<size_t> v4;

//     v1.reserve(sss/4);
//     v2.reserve(sss/4);
//     v3.reserve(sss/4);
//     v4.reserve(sss/4);
//     // size_t duplicates = 0;
//     // size_t sum = 0;
//     // std::vector<size_t> s;
//     // s.reserve(mmap.size() / sizeof(size_t));

//     const size_t* curr = reinterpret_cast<const size_t*>(mmap.data());

//     for (size_t i = 0; i < mmap.size(); i += sizeof(size_t) * 4) {
//         size_t el1 = *(curr);
//         size_t el2 = *(curr + 1);
//         size_t el3 = *(curr + 2);
//         size_t el4 = *(curr + 3);

//         curr += 4;

//         // bool res = binarySearch(reinterpret_cast<const size_t*>(mmap.data()), i / sizeof(size_t), el);
//         // if (res) {
//         //     duplicates++;
//         // }

//         v1.push_back(el1);
//         v2.push_back(el2);
//         v3.push_back(el3);
//         v4.push_back(el4);

//         // if (s.count(el) > 0) {
//         //     duplicates++;
//         // }

//     }

//     printf("Unique Count: %lu\n", v1.size() + v2.size() + v3.size() + v4.size());

//     // mmap.unmap();

//     // Basic file I/O
//     // {
//     //     FILE* file = fopen(path, "rb");

//     //     if (file == nullptr) {
//     //         printf("error opening file: %s\n", strerror(errno));
//     //         return errno;
//     //     }

//     //     struct stat st;
//     //     if (fstat(fileno(file), &st) != 0) {
//     //         printf("error getting file size: %s\n", strerror(errno));
//     //         return errno;
//     //     }

//     //     printf("Starting work on dataset with size: %lu\n", st.st_size);

//     //     const size_t sss = st.st_size / sizeof(size_t);
//     //     std::vector<size_t> v1;
//     //     std::vector<size_t> v2;
//     //     std::vector<size_t> v3;
//     //     std::vector<size_t> v4;

//     //     v1.reserve(sss/4);
//     //     v2.reserve(sss/4);
//     //     v3.reserve(sss/4);
//     //     v4.reserve(sss/4);

//     //     while (true) {
//     //         size_t n[4];
//     //         int res = fread(&n, sizeof(size_t), 4, file);
//     //         if (res != 4) {
//     //             if (feof(file)) {
//     //                 break;
//     //             } else {
//     //                 printf("error reading file: %s\n", strerror(errno));
//     //                 return errno;
//     //             }
//     //         }

//     //         v1.push_back(n[0]);
//     //         v2.push_back(n[1]);
//     //         v3.push_back(n[2]);
//     //         v4.push_back(n[3]);
//     //     }

//     //     printf("Unique elements: %lu\n", v1.size() + v2.size() + v3.size() + v4.size());

//     //     fclose(file);
//     // }

//     return 0;
// }

#include <dbms.h>

int main() {
    dbms::initSubmodules();

    {
        TRACE_BLOCK_CPU_TIME("Time Taken for this block: ");

        u64 counter = 0;
        while (counter++ < 10000000) {}
    }

    {
        TRACE_BLOCK_THROUGHPUT("Time Taken for second block: ", 60000);

        u64 counter = 0;
        while (counter++ < 100000000) {}
    }


    return 0;
}
