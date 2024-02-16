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
    std::vector<Column> columns;
};

struct Database {
    std::string name;
    std::filesystem::path path;
    std::vector<Table> tables;
};

void initSubmodules();

} // namespace dbms
