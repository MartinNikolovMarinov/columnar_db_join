#pragma once

#include <core.h>

#include <filesystem>
#include <vector>
#include <string>

namespace dbms {

struct DataSource;
struct Column;
struct ColumnNames;
struct JoinResult;

using ColumnGroup = std::vector<Column>;
using IndexTranslationTable = std::vector<std::pair<u64, u64>>;

struct DataSource {

    DataSource() : m_size(0), m_src(nullptr) {}
    DataSource(u64 size, const u64* src) : m_size(size), m_src(src) {}

    inline u64 operator[](u64 idx) const {
        Assert(idx < m_size, "Index out of bounds");
        return m_src[idx];
    }

    inline u64 size() const {
        return m_size;
    }

    inline bool empty() const {
        return m_size == 0;
    }

private:
    u64 m_size;
    const u64* m_src;
};

struct Column {

    Column()
        : m_size(0)
        , m_mappedFile(nullptr)
        , m_inMemoryData({}) {}
    Column(u64 size, const u64* mappedFile)
        : m_size(size)
        , m_mappedFile(mappedFile)
        , m_inMemoryData({}) {}
    Column(std::vector<u64>&& inMemoryData)
        : m_size(inMemoryData.size())
        , m_mappedFile(nullptr)
        , m_inMemoryData(std::move(inMemoryData)) {}

    inline DataSource data() const {
        if (m_mappedFile != nullptr) {
            return DataSource(m_size, m_mappedFile);
        }
        return DataSource(m_inMemoryData.size(), m_inMemoryData.data());
    }

    inline void append(u64 x) {
        Assert(m_mappedFile == nullptr, "Cannot append to a column that is backed by a file");
        m_inMemoryData.push_back(x);
        m_size++;
    }

private:
    u64 m_size;
    const u64* m_mappedFile;
    std::vector<u64> m_inMemoryData;
};

struct ColumnNames {
    std::vector<std::string> colNames;
    std::vector<std::string> valueColNames;
};
struct JoinResult {
    ColumnGroup columns;
    ColumnNames names;

    static JoinResult createFromNames(const ColumnNames& a, const ColumnNames& b);
};

void initSubmodules();

// void loadDatabase(const char* path, Database& db);
// void debug_printTable(dbms::Table& table);

// std::vector<dbms::Table> optimizeAlignmentExecutionOrder(std::vector<dbms::Table>&& tables);
// bool canBeAligned(const std::vector<dbms::Table>& tables);

/**
 * @brief Create a translation table to translate the index of the source table to the index of the destination table.
 *        This function assumes that the source and destination tables are sorted lexically.
 *        This function also assumes that the source and destination tables have only unique column names.
 *
 * @param from The source table column names.
 * @param to The destination table column names.
 *
 * @return A vector of pairs where the first element is the index of the source table and the second element is the index of the destination table.
*/
IndexTranslationTable createIndexTranslationTable(const ColumnNames& from, const ColumnNames& to);

/**
 * @brief FIXME: Write a description
*/
JoinResult hashJoin(const ColumnGroup& left,
                    const ColumnGroup& right,
                    const ColumnNames& leftColNames,
                    const ColumnNames& rightColNames);


/**
 * @brief FIXME: Write a description
*/
void sortColumns(ColumnGroup& cols);

} // namespace dbms
