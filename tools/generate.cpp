#include "tools.h"

#include <filesystem>
#include <vector>
#include <string>
#include <string_view>
#include <algorithm>

void generateRandomColumns(const char* path, std::vector<RndEntry>& entries) {
    namespace fs = std::filesystem;

    if (!fs::create_directories(path)) {
        fs::remove_all(path);
    }

    for (const auto& entry : entries) {
        std::string v = std::string(path) + std::string("/") + entry.columnName;
        fs::create_directories(v);

        if (entry.columnKeys.empty()) continue;

        addr_size entriesCount = addr_size(core::rndI64(entry.minEntries, entry.maxEntries));

        std::vector<addr_size> keyIndices(entriesCount);

        // First key is clustered index:
        {
            std::string v2 = v + std::string("/") + entry.columnKeys[0];
            FILE* file = fopen(v2.data(), "wb");
            Assert(file);
            defer { Assert(fclose(file) == 0); };

            addr_size actualIdx = 0;

            for (addr_size i = 0; i < entriesCount; i++) {
                if (core::rndI64() & 1) { actualIdx++; }
                keyIndices[i] = actualIdx;
                auto written = fwrite(&actualIdx, sizeof(addr_size), 1, file);
                Assert(written == 1);
            }

            logInfo("Generated %s with %zu entries", v2.data(), entriesCount);
        }

        for (addr_size j = 1; j < entry.columnKeys.size(); j++) {
            const auto& key = entry.columnKeys[j];
            std::string v2 = v + std::string("/") + key;
            FILE* file = fopen(v2.data(), "wb");
            Assert(file);

            addr_size value = 0;
            bool prevWasSame = false;

            for (addr_size k = 0; k < entriesCount - 1; k++) {
                if (keyIndices[k] == keyIndices[k + 1]) {
                    value++;
                    prevWasSame = true;
                }
                else if (prevWasSame) {
                    value++;
                    prevWasSame = false;
                }
                else {
                    value = core::rndI64(entry.minValues, entry.maxValues);
                }

                keyIndices[k] = value;

                auto written = fwrite(&value, sizeof(addr_size), 1, file);
                Assert(written == 1);
            }

            keyIndices[entriesCount - 1] = value;
            auto written = fwrite(&value, sizeof(addr_size), 1, file);
            Assert(written == 1);

            Assert(fclose(file) == 0);

            logInfo("Generated %s with %zu entries", v2.data(), entriesCount);
        }
    }
}

void generateExample1() {
    std::vector<RndEntry> entries = {
        {"column1", {"key_A.bin", "key_B.bin", "value.bin"}, 0, 100, 10, 100},
        {"column2", {"key_A.bin", "key_C.bin", "value.bin"}, 0, 100, 10, 100},
        {"column3", {"key_A.bin", "key_D.bin", "key_E.bin", "value.bin"}, 0, 3, 10, 100}
    };
    generateRandomColumns(DBMS_DATA_PATH"/ex1_all_joins_on_clustered_idx", entries);
}
