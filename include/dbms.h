#pragma once

#include <core.h>
#include <data_source.h>

#include <filesystem>
#include <vector>
#include <string>

namespace dbms {

constexpr const char* DBMS_DATA_FILE_EXTENSION = ".bin";
constexpr const char* DBMS_DATA_COLUMN_PREFIX = "key_";

template <DataSourceConcept T>
struct ColumnDataT {
    T memorySrc;
};

using ColumnData = ColumnDataT<MemoryMappedFile>;

struct Table {
    std::string name;
    std::filesystem::path path;
    std::vector<std::string> columnNames;
    std::vector<ColumnData> columns; // First column is the clustered index and the last column is the value. Columns are lexically ordered.
};

struct Database {
    std::string name;
    std::filesystem::path path;
    std::vector<Table> tables;
};

void initSubmodules();

void loadDatabase(const char* path, Database& db);
void debug_printTable(dbms::Table& table);

std::vector<dbms::Table> optimizeAlignmentExecutionOrder(std::vector<dbms::Table>&& tables);
bool canBeAligned(const std::vector<dbms::Table>& tables);

} // namespace dbms
