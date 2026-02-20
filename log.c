#include <stdarg.h>
#include <stdio.h>

#include "log.h"

LogEntry logs[LOG_CAPACITY];
int log_count = 0;

void log_message(LogLevel level, const char *fmt, ...) {
  if (log_count < LOG_CAPACITY) {
    log_count++;
  }
  for (int i = log_count - 1; i > 0; i--) {
    logs[i] = logs[i - 1];
  }

  logs[0].level = level;
  va_list args;
  va_start(args, fmt);
  vsnprintf(logs[0].msg, 127, fmt, args);
  va_end(args);
}
