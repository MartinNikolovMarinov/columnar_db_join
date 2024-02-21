#include <core/measurements.h>

namespace core {

void Timer::start() {
    begin = std::chrono::high_resolution_clock::now();
}

void Timer::update() {
    delta = std::chrono::high_resolution_clock::now() - begin;
}

void Timer::reset() {
    begin = std::chrono::high_resolution_clock::now();
}

std::string Timer::toString() {
    std::string result;
    result.resize(256);

    if (delta >= std::chrono::hours(1)) {
        i32 hours = i32(std::chrono::duration_cast<std::chrono::hours>(delta).count());
        i32 minutes = i32(std::chrono::duration_cast<std::chrono::minutes>(delta).count() % 60);
        i32 seconds = i32(std::chrono::duration_cast<std::chrono::seconds>(delta).count() % 60);
        i64 milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(delta).count() % 1000;
        sprintf(&result[0], "%02dh %02dm %02ds %lldms", hours, minutes, seconds, milliseconds);
    }
    else if (delta >= std::chrono::minutes(1)) {
        i32 minutes = i32(std::chrono::duration_cast<std::chrono::minutes>(delta).count());
        i32 seconds = i32(std::chrono::duration_cast<std::chrono::seconds>(delta).count() % 60);
        i64 milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(delta).count() % 1000;
        sprintf(&result[0], "%02dm %02ds %lldms", minutes, seconds, milliseconds);
    }
    else if (delta >= std::chrono::seconds(1)) {
        i32 seconds = i32(std::chrono::duration_cast<std::chrono::seconds>(delta).count());
        i64 milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(delta).count() % 1000;
        sprintf(&result[0], "%02ds %lldms", seconds, milliseconds);
    }
    else if (delta >= std::chrono::milliseconds(1)) {
        i64 milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(delta).count();
        sprintf(&result[0], "%lldms", milliseconds);
    }
    else {
        i64 nanoseconds = std::chrono::duration_cast<std::chrono::nanoseconds>(delta).count();
        sprintf(&result[0], "%lldns", nanoseconds);
    }

    return result;
}

void Throughput::start(addr_size byteCount) {
    m_timer.start();
    m_byteCount = byteCount;
}

void Throughput::update(addr_size byteCount) {
    m_byteCount += byteCount;
    m_timer.update();
}

void Throughput::reset() {
    m_timer.reset();
    m_byteCount = 0;
}

addr_size Throughput::unprocessed() const { return m_byteCount; }

f64 Throughput::getThroughputBps() const {
    return f64(m_byteCount) / f64(m_timer.delta.count());
}

f64 Throughput::getThroughputKBps() const {
    return (f64(m_byteCount) / f64(CORE_KILOBYTE)) / f64(m_timer.delta.count());
}

f64 Throughput::getThroughputMBps() const {
    return (f64(m_byteCount) / f64(CORE_MEGABYTE)) / f64(m_timer.delta.count());
}

f64 Throughput::getThroughputGBps() const {
    return (f64(m_byteCount) / f64(CORE_GIGABYTE)) / f64(m_timer.delta.count());
}

std::string Throughput::toString() {
    std::string result;
    result.resize(256);

    f64 bps = getThroughputBps();

    if (bps >= CORE_GIGABYTE) {
        sprintf(&result[0], "%.2f GB/s", getThroughputGBps());
    }
    else if (bps >= CORE_MEGABYTE) {
        sprintf(&result[0], "%.2f MB/s", getThroughputMBps());
    }
    else if (bps >= CORE_KILOBYTE) {
        sprintf(&result[0], "%.2f KB/s", getThroughputKBps());
    }
    else {
        sprintf(&result[0], "%.2f B/s", getThroughputBps());
    }

    return result;
}

} // namespace core


