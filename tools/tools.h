#pragma once

#include <core.h>

#include <string>
#include <vector>

struct RndEntry {
    std::string columnName;
    std::vector<std::string> columnKeys;
    addr_size minValues;
    addr_size maxValues;
    addr_size minEntries;
    addr_size maxEntries;
};

void generateRandomColumns(const char* path, std::vector<RndEntry>& entries);
void generatedTablesAreValid(std::vector<RndEntry>& entries);

void generateExample1();
