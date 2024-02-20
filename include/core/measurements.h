#pragma once

#include "types.h"
#include "utils.h"

#include <string>
#include <chrono>

namespace core {

struct Timer {
    std::chrono::high_resolution_clock::time_point begin;
    std::chrono::duration<double> delta;

    void start();
    void update();
    void reset();
    std::string toString();
};

struct Throughput {
    void start(addr_size byteCount);
    void update(addr_size byteCount);
    void reset();
    addr_size unprocessed() const;

    f64 getThroughputBps() const;
    f64 getThroughputKBps() const;
    f64 getThroughputMBps() const;
    f64 getThroughputGBps() const;

    std::string toString();

private:
    Timer m_timer;
    addr_size m_byteCount;
};

#define CONCATENATE_DIRECT(s1, s2) s1##s2
#define CONCATENATE(s1, s2) CONCATENATE_DIRECT(s1, s2)

#define TRACE_BLOCK_CPU_TIME(prefix_msg) \
    core::Timer CONCATENATE(timer, __LINE__); \
    CONCATENATE(timer, __LINE__).start(); \
    defer { \
        CONCATENATE(timer, __LINE__).update(); \
        logTrace(prefix_msg": %s", CONCATENATE(timer, __LINE__).toString().data()); \
    }; \

#define INFO_BLOCK_CPU_TIME(prefix_msg) \
    core::Timer CONCATENATE(timer, __LINE__); \
    CONCATENATE(timer, __LINE__).start(); \
    defer { \
        CONCATENATE(timer, __LINE__).update(); \
        logInfo(prefix_msg": %s", CONCATENATE(timer, __LINE__).toString().data()); \
    }; \

#define TRACE_BLOCK_THROUGHPUT(prefix_msg, byteCount) \
    core::Throughput throughput; \
    throughput.start(byteCount); \
    defer { \
        throughput.update(byteCount); \
        logTrace(prefix_msg": %s", throughput.toString().data()); \
    }; \

// #define TRACE_BLOCK_THROUGHPUT(prefix_msg, byteCount) _TRACE_BLOCK_THROUGHPUT(prefix_msg, byteCount, __LINE__)

} // namespace core

