#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include "helpers.h"
#include "log.h"

Logs logs = {0};

void log_message(LogLevel level, const char *fmt, ...) {
  char msg[LOG_MAX_LENGTH + 1];

  va_list args;
  va_start(args, fmt);
  vsnprintf(msg, LOG_MAX_LENGTH, fmt, args);
  va_end(args);

  Log log = {0};
  log.level = level;
  log.msg = strdup(msg);

  da_append(&logs, log);
}

void free_logs() {
  da_free(logs);
  logs.count = 0;
  logs.capacity = 0;
}
