#include <memory_mapped_file.h>

#include <system_error>

namespace dbms {

bool openMemoryMappedFile(std::string_view path, MemoryMappedSrc& src) {
    std::error_code err;
    src = mio::make_mmap_source(path.data(), err);
    if (err) {
        logFatal("Error Mapping File: %s, exiting...", err.message().c_str());
        return false;
    }
    return true;
}

} // namespace dbms

