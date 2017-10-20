#ifndef UT_NOTIFY_H
#define UT_NOTIFY_H

#include <stdio.h>

typedef enum {
	LV_FATAL = 0,
	LV_ERROR = 1,
	LV_WARN = 2,
	LV_INFO = 3,
	LV_DEBUG = 4,
	LV_TRACE = 5
} severity_t;

#define UTILS_NOTIFY_LEVEL(a) utils_notify_level(a)
#define UTILS_NOTIFY_FILENAME(a) utils_notify_filename(a)
#define UTILS_NOTIFY_STARTUP() utils_notify_startup()
#define UTILS_NOTIFY_SHUTDOWN() utils_notify_shutdown()

#if defined(__cplusplus)

#include <string>
#include <sstream>
#include <iostream>

#ifdef WIN32
#define __func__ __FUNCTION__
#endif

#define UT_NOTIFY(sev, msg)                                 \
    do {                                                    \
        std::stringstream buf;                              \
        buf << msg;                                         \
        utils_notify_va(sev, __FILE__, __func__, __LINE__, 1, buf.str().c_str());   \
    }                                                       \
    while(0)

#define UT_NOTIFY_NO_PRFX(msg)							\
    do {										\
        std::stringstream buf;					\
        buf << msg;								\
        utils_notify_out(buf.str().c_str());	\
    }											\
    while(0)

#define UT_NOTIFY_NL(sev, msg)								\
    do {													\
        std::stringstream buf;								\
        buf << msg;											\
        utils_notify_va(sev, __FILE__, __func__, __LINE__, 0, buf.str().c_str());	\
    }														\
    while(0)

#else   // C
#define UT_NOTIFY(sev, ...)                                 \
            utils_notify_va(sev, __FILE__, __func__, __LINE__, 1, __VA_ARGS__)
#define UT_NOTIFY_NL(sev, ...)                              \
            utils_notify_va(sev, __FILE__, __func__, __LINE__, 0, __VA_ARGS__)
#define UT_NOTIFY_NO_PRFX(...)                         \
            utils_notify_out(__VA_ARGS__)
#endif  // __cplusplus

#if defined(__cplusplus)
extern "C" {
#endif
extern int utils_notify_to_file(const int* value);
extern const char* utils_notify_filename(const char* filename);
extern severity_t utils_notify_level(const severity_t* value);
extern void utils_notify_startup();
extern void utils_notify_shutdown();
extern void utils_notify_va(const severity_t sev, const char* filename,
		const char* func, unsigned int line, unsigned int newline, const char *fmt, ...);
extern void utils_notify_out(const char* fmt, ...);
extern const char* utils_notify_strlevel(const severity_t t);

#if defined(__cplusplus)
}
#endif
#endif  // UT_NOTIFY_H
