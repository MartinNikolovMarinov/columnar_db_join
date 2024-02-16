#pragma once

#include <core.h>

#include <filesystem>
#include <vector>
#include <string>

#include <mio/mio.hpp>

namespace dbms {

struct Column {
    std::string name;
    mio::mmap_source memorySrc;
};

struct Table {
    std::string name;
    std::filesystem::path path;
    std::vector<Column> columns; // First column is the clustered index and the last column is the value. Columns are lexically ordered.
};

struct Database {
    std::string name;
    std::filesystem::path path;
    std::vector<Table> tables;
};

void initSubmodules();

std::vector<dbms::Table> optimizeExecutionOrder(std::vector<dbms::Table>&& tables);
bool isJoinPossible(const std::vector<dbms::Table>& tables);

} // namespace dbms
