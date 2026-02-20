#ifndef LOG_H
#define LOG_H

#include <ncurses.h>

typedef enum { LOG_INFO, LOG_WARNING, LOG_ERROR } LogLevel;

typedef struct {
  LogLevel level;
  char msg[128];
} LogEntry;

#define LOG_CAPACITY 5
#define log_add(level, fmt, ...) log_message(level, fmt, ##__VA_ARGS__)

void log_message(LogLevel level, const char *fmt, ...);

extern LogEntry logs[LOG_CAPACITY];
extern int log_count;

#endif
