#ifdef OS_WIN
#include "notify_windows.h"
#include <QtCore/QDebug>

void utils_notify_va(const severity_t sev, const char *filename, const char *func, unsigned int line, unsigned int newline, const char *fmt, ...)
{
    Q_UNUSED(filename)
    Q_UNUSED(func)
    Q_UNUSED(line)
    Q_UNUSED(newline)

    qInfo() << QString::fromStdString(utils_notify_strlevel(sev)) << QString::fromStdString(fmt);
}

void utils_notify_out(const char *fmt)
{
    qInfo() << QString::fromStdString(fmt);
}

void utils_notify_shutdown()
{

}

void utils_notify_startup()
{

}

severity_t utils_notify_level(const severity_t *value)
{
    Q_UNUSED(value)
    return LV_INFO;
}

const char *utils_notify_filename(const char *filename)
{
    Q_UNUSED(filename)
    return 0x00;
}

int utils_notify_to_file(const int *value)
{
    Q_UNUSED(value)
    return 0;
}

const char* utils_notify_strlevel(const severity_t t)
{
    switch(t) {
    case LV_FATAL:
        return "FATAL";
    case LV_ERROR:
        return "ERROR";
    case LV_WARN:
        return "WARN";
    case LV_INFO:
        return "INFO";
    case LV_DEBUG:
        return "DEBUG";
    case LV_TRACE:
        return "TRACE";
    default:
        return "UNDEFINED LEVEL";
    }
}
#endif
