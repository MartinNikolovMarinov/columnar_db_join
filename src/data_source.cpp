#include <data_source.h>

#include <mio/mio.hpp>

namespace dbms {

struct MemoryMappedFile::MemoryMappedFileImpl {
    inline i32 open(const char* path, addr_size offset) {
        std::error_code error;
        mappedFile = mio::make_mmap_source(path, offset, mio::map_entire_file, error);
        if (error) {
            logErr("Failed to open file: %s", error.message());
            return error.value();
        }
        return 0;
    }

    inline i32 close() {
        mappedFile.unmap();
        return 0;
    }

    inline const void* data(addr_size at, addr_size& max) {
        if (at >= mappedFile.size()) {
            max = 0;
            return nullptr;
        }
        max = mappedFile.size();
        return mappedFile.data() + at;
    }

    inline addr_size size() const {
        return mappedFile.size();
    }

    mio::mmap_source mappedFile;
};

MemoryMappedFile::MemoryMappedFile() : m_impl(std::make_unique<MemoryMappedFileImpl>()) {}
MemoryMappedFile::MemoryMappedFile(MemoryMappedFile&& other) = default;
MemoryMappedFile::~MemoryMappedFile() = default;

i32 MemoryMappedFile::open(const char* path, addr_size offset) {
    m_impl = std::make_unique<MemoryMappedFileImpl>();
    return m_impl->open(path, offset);
}

i32 MemoryMappedFile::close() {
    return m_impl->close();
}

const void* MemoryMappedFile::data(addr_size at, addr_size& max) {
    return m_impl->data(at, max);
}

addr_size MemoryMappedFile::size() const {
    return m_impl->size();
}

} // namespace dbms
