//
// Created by aogun on 2020/5/26.
//

#ifndef MW_SC_LOG_H
#define MW_SC_LOG_H

#ifdef __cplusplus
extern "C"
{
#endif
#ifndef MM_API
#ifdef _WIN32
#ifndef __MINGW__
            #ifdef MM_DYNAMIC
                #ifdef MM_EXPORTS
                    #define MM_API __declspec(dllexport)
                #else
                    #define MM_API __declspec(dllimport)
                #endif
            #else
                #define MM_API
            #endif
        #else
            #define MM_API
        #endif
#else
#define MM_API __attribute__ ((visibility("default")))
#endif
#endif

MM_API void mm_log_info(const char * format, const char * file, int line, const char * func, ...);
MM_API void mm_log_warn(const char * format, const char * file, int line, const char * func, ...);
MM_API void mm_log_error(const char * format, const char * file, int line, const char * func, ...);
MM_API void mm_log_init();

#define MM_LOG_INFO(format, ...) \
    mm_log_info(format, __FILE__, __LINE__, __func__, ##__VA_ARGS__)

#define MM_LOG_WARN(format, ...) \
    mm_log_warn(format, __FILE__, __LINE__, __func__, ##__VA_ARGS__)

#define MM_LOG_ERROR(format, ...) \
    mm_log_error(format, __FILE__, __LINE__, __func__, ##__VA_ARGS__)


#ifdef __cplusplus
}
#endif
#endif //MW_SC_LOG_H
