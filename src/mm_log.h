//
// Created by aogun on 2020/5/26.
//

#ifndef MW_SC_LOG_H
#define MW_SC_LOG_H

#ifdef __cplusplus
extern "C"
{
#endif


typedef void (*output_func)(const char* fmt, ...);

void mm_log_info(const char * format, const char * file, int line, const char * func, ...);
void mm_log_warn(const char * format, const char * file, int line, const char * func, ...);
void mm_log_error(const char * format, const char * file, int line, const char * func, ...);
void mm_log_init(output_func func);

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
