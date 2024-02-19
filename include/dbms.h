#pragma once

#include <core.h>
#include <memory_mapped_file.h>

#include <filesystem>
#include <vector>
#include <string>

namespace dbms {

struct Table;
struct Database;
struct DataSource;
struct Column;
struct ColumnNames;
struct JoinResult;

using ColumnGroup = std::vector<Column>;
using IndexTranslationTable = std::vector<std::pair<u64, u64>>;
using WriteOrder = std::pair<IndexTranslationTable, IndexTranslationTable>;

void initSubmodules();
void debug_printColumnGroup(const ColumnGroup& cols, const ColumnNames& columnNames);

struct DataSource {

    constexpr DataSource() : m_size(0), m_src(nullptr) {}
    constexpr DataSource(u64 size, const u64* src) : m_size(size), m_src(src) {}

    constexpr inline u64 operator[](u64 idx) const {
        Assert(idx < m_size, "Index out of bounds");
        return m_src[idx];
    }

    constexpr inline u64 size() const {
        return m_size;
    }

    constexpr inline bool empty() const {
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

    inline void reserve(u64 size) {
        Assert(m_mappedFile == nullptr, "Cannot reserve space for a column that is backed by a file");
        m_inMemoryData.reserve(size);
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

struct Database {
    struct Table {
        std::string name;
        std::string path;
        ColumnNames names;
        ColumnGroup columns;
        std::vector<MemoryMappedSrc> mappedFiles;
    };

    std::string path;
    std::string name;
    std::vector<Table> tables;

    static constexpr std::string_view DBMS_DATA_COLUMN_PREFIX = "key_";
    static constexpr std::string_view DBMS_DATA_FILE_EXTENSION = ".bin";
};

bool loadDatabase(const char* path, Database& db);

struct JoinResult {
    ColumnGroup columns;
    ColumnNames names;

    static JoinResult createFromNames(const ColumnNames& a, const ColumnNames& b);
};

u64 sumSquared(JoinResult& cols);

void appendRowToResult(JoinResult& result,
                       const ColumnNames& leftColNames,
                       const ColumnNames& rightColNames,
                       const ColumnGroup& left,
                       const ColumnGroup& right,
                       u64 leftRowIdx, u64 rightRowIdx);

bool checkSecondaryKeys(const ColumnNames& leftColNames,
                        const ColumnNames& rightColNames,
                        const ColumnGroup& left,
                        const ColumnGroup& right,
                        u64 leftRowIdx, u64 rightRowIdx);
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

WriteOrder createTableWriteOrder(const ColumnNames& a, const ColumnNames& b, const ColumnNames& unique);

void sortDataInColumns(ColumnGroup& cols);

JoinResult hashJoin(const ColumnGroup& left,
                    const ColumnGroup& right,
                    const ColumnNames& leftColNames,
                    const ColumnNames& rightColNames);

JoinResult crossJoin(const ColumnGroup& left,
                     const ColumnGroup& right,
                     const ColumnNames& leftColNames,
                     const ColumnNames& rightColNames);

JoinResult mergeJoin(const ColumnGroup& left,
                     const ColumnGroup& right,
                     const ColumnNames& leftColNames,
                     const ColumnNames& rightColNames);

JoinResult binarySearchJoin(const ColumnGroup& left,
                            const ColumnGroup& right,
                            const ColumnNames& leftColNames,
                            const ColumnNames& rightColNames);

} // namespace dbms
