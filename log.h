#ifndef H_LOG_INCLUDE
#define H_LOG_INCLUDE

#include <stdio.h>

// 0 : FATAL
// 1 : ERROR
// 2 : WARNING
// 3 : INFO
// 4 : DEBUG

#ifndef LOG_LEVEL
#define LOG_LEVEL 3
#endif

#define LOG_T(prefix, level, ...) if(level < LOG_LEVEL) printf(prefix __VA_ARGS__)

#define FATAL(...) LOG_T("fatal : ", 0, __VA_ARGS__)
#define ERROR(...) LOG_T("error : ", 1, __VA_ARGS__)
#define WARN(...) LOG_T("warning : ", 2, __VA_ARGS__)
#define INFO(...) LOG_T("info : ", 3, __VA_ARGS__)
#define DEBUG(...) LOG_T("debug : ", 4, __VA_ARGS__)

#endif //H_LOG_INCLUDE