//
// Created by aogun on 2020/5/26.
//

#include <cstdarg>
#include <cstdio>
#include "mm_log.h"
#include <sys/timeb.h>
#include <ctime>
#include <string>


#define LOG_SIZE 1024
static output_func m_func = nullptr;

inline std::tm localtime(std::time_t const & time) {
    std::tm tm_snapshot;
#if (defined(__MINGW32__) || defined(__MINGW64__))
    memcpy(&tm_snapshot, ::localtime(&time), sizeof(std::tm));
#elif (defined(WIN32) || defined(_WIN32) || defined(__WIN32__))
    localtime_s(&tm_snapshot, &time);
#else
    localtime_r(&time, &tm_snapshot); // POSIX
#endif
    return tm_snapshot;
}

void mm_log_init(output_func func) {
#ifdef USE_SPDLOG
#else
    setbuf(stdout, NULL);
#endif
    m_func = func;
}

const char * strip_file_name(const char *filename) {
    if (!filename) return nullptr;
    std::string name = filename;
    auto index = name.find_last_of("\\/");
    if (index != std::string::npos) {
        return filename + index + 1;
    }
    return filename;
}

void mm_log_info(const char * format, const char * file, int line, const char * func, ...) {
    char sz[LOG_SIZE];
    va_list ap;
    va_start(ap, func);
    vsnprintf(sz, LOG_SIZE, format, ap);
    va_end(ap);
#ifdef USE_SPDLOG
    const auto logger = spdlog::default_logger_raw();
    auto level = (spdlog::level::level_enum)SPDLOG_LEVEL_INFO;                                                                                           \
    logger->log(spdlog::source_loc{file, line, func}, level, sz);
#else
    struct  timeb   tb;
    ftime(&tb);
    std::time_t t = std::time(NULL);
    std::tm lt = localtime(t);
    char buffer[20];
    sprintf(buffer, "%02d-%02d %02d:%02d:%02d.%03d",
            lt.tm_mon+1, lt.tm_mday, lt.tm_hour, lt.tm_min, lt.tm_sec, tb.millitm);
    if (m_func) {
        m_func("[%s] [info] [%s:%d] %s\n", buffer, strip_file_name(file), line, sz);
    }

    printf("[%s] [info] [%s:%d] %s\n", buffer, strip_file_name(file), line, sz);
#endif
}
void mm_log_warn(const char * format, const char * file, int line, const char * func, ...) {
    char sz[LOG_SIZE];
    va_list ap;
    va_start(ap, func);
    vsnprintf(sz, LOG_SIZE, format, ap);
    va_end(ap);
#ifdef USE_SPDLOG
    const auto logger = spdlog::default_logger_raw();
    auto level = (spdlog::level::level_enum)SPDLOG_LEVEL_WARN;                                                                                           \
    logger->log(spdlog::source_loc{file, line, func}, level, sz);
#else
    struct  timeb   tb;
    ftime(&tb);
    std::time_t t = std::time(NULL);
    std::tm lt = localtime(t);
    char buffer[20];
    sprintf(buffer, "%02d-%02d %02d:%02d:%02d.%03d",
            lt.tm_mon+1, lt.tm_mday, lt.tm_hour, lt.tm_min, lt.tm_sec, tb.millitm);
    if (m_func) {
        m_func("[%s] [warn] [%s:%d] %s\n", buffer, strip_file_name(file), line, sz);
    }

    printf("[%s] [warn] [%s:%d] %s\n", buffer, strip_file_name(file), line, sz);

#endif
}
void mm_log_error(const char * format, const char * file, int line, const char * func, ...) {
    char sz[LOG_SIZE];
    va_list ap;
    va_start(ap, func);
    vsnprintf(sz, LOG_SIZE, format, ap);
    va_end(ap);
#ifdef USE_SPDLOG
    const auto logger = spdlog::default_logger_raw();
    auto level = (spdlog::level::level_enum)SPDLOG_LEVEL_ERROR;                                                                                           \
    logger->log(spdlog::source_loc{file, line, func}, level, sz);
#else
    struct  timeb   tb;
    ftime(&tb);
    std::time_t t = std::time(NULL);
    std::tm lt = localtime(t);
    char buffer[20];
    sprintf(buffer, "%02d-%02d %02d:%02d:%02d.%03d",
            lt.tm_mon+1, lt.tm_mday, lt.tm_hour, lt.tm_min, lt.tm_sec, tb.millitm);
    if (m_func) {
        m_func("[%s] [error] [%s:%d] %s\n", buffer, strip_file_name(file), line, sz);
    }

    printf("[%s] [error] [%s:%d] %s\n", buffer, strip_file_name(file), line, sz);

#endif
}