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

void log_init();
void log_message(LogLevel level, const char *fmt, ...);
void log_render();
void log_resize();
void log_cleanup();

#endif
