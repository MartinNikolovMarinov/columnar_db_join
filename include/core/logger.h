#pragma once

#include "types.h"
#include "ansi.h"

namespace core {

enum struct LogLevel : u8 {
    L_TRACE = 0,
    L_DEBUG,
    L_INFO,
    L_WARNING,
    L_ERROR,
    L_FATAL,

    SENTINEL
};

enum struct LogTag : u8 {
    T_ENGINE = 0,

    SENTINEL
};

enum struct LogSpecialMode : u8 {
    NONE = 0,

    SECTION_TITLE,

    SENTINEL
};

bool initLoggingSystem(LogLevel minLogLevel, LogTag* tagsToIgnore, addr_size tagsToIgnoreCount);
void shutdownLoggingSystem();

void __log(LogTag tag, LogLevel level, LogSpecialMode mode, const char* funcName, const char* format, ...);
void muteLogger(bool mute);

#define logTrace(format, ...) __log(core::LogTag::T_ENGINE, core::LogLevel::L_TRACE,   core::LogSpecialMode::NONE, __func__, format, ##__VA_ARGS__)
#define logDebug(format, ...) __log(core::LogTag::T_ENGINE, core::LogLevel::L_DEBUG,   core::LogSpecialMode::NONE, __func__, format, ##__VA_ARGS__)
#define logInfo(format, ...)  __log(core::LogTag::T_ENGINE, core::LogLevel::L_INFO,    core::LogSpecialMode::NONE, __func__, format, ##__VA_ARGS__)
#define logWarn(format, ...)  __log(core::LogTag::T_ENGINE, core::LogLevel::L_WARNING, core::LogSpecialMode::NONE, __func__, format, ##__VA_ARGS__)
#define logErr(format, ...)   __log(core::LogTag::T_ENGINE, core::LogLevel::L_ERROR,   core::LogSpecialMode::NONE, __func__, format, ##__VA_ARGS__)
#define logFatal(format, ...) __log(core::LogTag::T_ENGINE, core::LogLevel::L_FATAL,   core::LogSpecialMode::NONE, __func__, format, ##__VA_ARGS__)

#define logTraceTagged(tag, format, ...) __log(tag, core::LogLevel::L_TRACE,   core::LogSpecialMode::NONE, __func__, format, ##__VA_ARGS__)
#define logDebugTagged(tag, format, ...) __log(tag, core::LogLevel::L_DEBUG,   core::LogSpecialMode::NONE, __func__, format, ##__VA_ARGS__)
#define logInfoTagged(tag, format, ...)  __log(tag, core::LogLevel::L_INFO,    core::LogSpecialMode::NONE, __func__, format, ##__VA_ARGS__)
#define logWarnTagged(tag, format, ...)  __log(tag, core::LogLevel::L_WARNING, core::LogSpecialMode::NONE, __func__, format, ##__VA_ARGS__)
#define logErrTagged(tag, format, ...)   __log(tag, core::LogLevel::L_ERROR,   core::LogSpecialMode::NONE, __func__, format, ##__VA_ARGS__)
#define logFatalTagged(tag, format, ...) __log(tag, core::LogLevel::L_FATAL,   core::LogSpecialMode::NONE, __func__, format, ##__VA_ARGS__)

#define logSectionTitleTraceTagged(tag, format, ...) __log(tag, core::LogLevel::L_TRACE,   core::LogSpecialMode::SECTION_TITLE, __func__, format, ##__VA_ARGS__)
#define logSectionTitleDebugTagged(tag, format, ...) __log(tag, core::LogLevel::L_DEBUG,   core::LogSpecialMode::SECTION_TITLE, __func__, format, ##__VA_ARGS__)
#define logSectionTitleInfoTagged(tag, format, ...)  __log(tag, core::LogLevel::L_INFO,    core::LogSpecialMode::SECTION_TITLE, __func__, format, ##__VA_ARGS__)
#define logSectionTitleWarnTagged(tag, format, ...)  __log(tag, core::LogLevel::L_WARNING, core::LogSpecialMode::SECTION_TITLE, __func__, format, ##__VA_ARGS__)
#define logSectionTitleErrTagged(tag, format, ...)   __log(tag, core::LogLevel::L_ERROR,   core::LogSpecialMode::SECTION_TITLE, __func__, format, ##__VA_ARGS__)
#define logSectionTitleFatalTagged(tag, format, ...) __log(tag, core::LogLevel::L_FATAL,   core::LogSpecialMode::SECTION_TITLE, __func__, format, ##__VA_ARGS__)

} // namespace stlv
