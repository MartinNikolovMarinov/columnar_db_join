#include <core.h>
#include <mio/mio.hpp>

namespace dbms {

using MemoryMappedSrc = mio::mmap_source;

bool openMemoryMappedFile(std::string_view path, MemoryMappedSrc& src);

} // namespace dbms

