#ifndef LOG_H
#define LOG_H

#include <ncurses.h>

#define LOG_MAX_LENGTH 128
#define LOG_UI_CAPACITY 8

#define info(fmt, ...) log_message(LOG_INFO, fmt, ##__VA_ARGS__)
#define warn(fmt, ...) log_message(LOG_WARNING, fmt, ##__VA_ARGS__)
#define error(fmt, ...) log_message(LOG_ERROR, fmt, ##__VA_ARGS__)

typedef enum { LOG_INFO, LOG_WARNING, LOG_ERROR } LogLevel;

typedef struct {
  LogLevel level;
  const char *msg;
} Log;

typedef struct {
  Log *items;
  size_t capacity;
  size_t count;
} Logs;

void log_message(LogLevel level, const char *fmt, ...);
void free_logs();

extern Logs logs;

#endif
