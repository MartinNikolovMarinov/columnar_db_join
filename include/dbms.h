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

// void loadDatabase(const char* path, Database& db);
// void debug_printTable(dbms::Table& table);

// std::vector<dbms::Table> optimizeAlignmentExecutionOrder(std::vector<dbms::Table>&& tables);
// bool canBeAligned(const std::vector<dbms::Table>& tables);

/**
 * @brief Create a translation table to translate the index of the source table to the index of a destination table.
 *        This function assumes that the source and destination tables are sorted lexically.
 *        This function also assumes that the source and destination tables have only unique column names.
 *
 * @param from The source table column names.
 * @param fromCount The number of to take from the source table.
 * @param to The destination table column names.
 * @param toCount The number of to take from the destination table.
 *
 * @return A vector of pairs where the first element is the index of the source table and the second element is the index of the destination table.
*/
std::vector<std::pair<u64, u64>> createIndexTranslationTable(const std::vector<std::string>& from, u64 fromCount,
                                                             const std::vector<std::string>& to, u64 toCount);


} // namespace dbms
