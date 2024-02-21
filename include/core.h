#if defined(_WIN32) || defined(WIN32) || defined(__CYGWIN__) || defined(__MINGW32__) || defined(__BORLANDC__)
    // Windows defines macros for min and max. Which is very stupid.
    #define NOMINMAX 1
#endif

#include <core/types.h>
#include <core/ansi.h>
#include <core/utils.h>
#include <core/logger.h>
#include <core/measurements.h>
#include <core/random.h>
#include <core/stl_extensions.h>
