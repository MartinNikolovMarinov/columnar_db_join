#pragma once

#include <core.h>

#include <type_traits>
#include <memory>
#include <limits>

namespace dbms {

template <typename T>
concept CloserConcept = requires (T t) {
    // i32 close();
    { t.close() } -> std::same_as<i32>;
};

template <typename T>
concept DataSourceConcept = CloserConcept<T> && requires (T t) {
    // i32 open(const char* path, addr_size offset);
    { t.open(std::declval<const char*>(), std::declval<addr_size>()) } -> std::same_as<i32>;
    // const void* data(addr_size at, addr_size& max);
    { t.data(std::declval<addr_size>(), std::declval<addr_size&>()) } -> std::same_as<const void*>;
    // addr_size size();
    { t.size() } -> std::same_as<addr_size>;
};

struct MemoryMappedFile {
    MemoryMappedFile();
    MemoryMappedFile(MemoryMappedFile&& other);
    ~MemoryMappedFile();

    i32 open(const char* path, addr_size offset);
    i32 close();
    const void* data(addr_size at, addr_size& max);
    addr_size size() const;

private:
    struct MemoryMappedFileImpl;
    std::unique_ptr<MemoryMappedFileImpl> m_impl;
};

static_assert(DataSourceConcept<MemoryMappedFile>);

} // namespace name
