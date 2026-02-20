#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include "log.h"
#include "options.h"

typedef struct {

  LogLevel level;

  char msg[128];
} LogEntry;

LogEntry logs[LOG_CAPACITY];
int log_count = 0;
WINDOW *log_win = NULL;

void log_init() {
  int height = LOG_CAPACITY + 1;
  log_win = newwin(height, COLS, LINES - height, 0);
}

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

void log_render() {
  if (!log_win || !current_options.show_log)
    return;

  werase(log_win);
  mvwhline(log_win, 0, 0, ACS_HLINE, COLS);

  int display_row = 1; // Start rendering below the HLINE

  // Iterate through our stored logs
  for (int i = 0; i < log_count; i++) {
    // FILTER: Skip if message level is lower than setting
    if (logs[i].level < current_options.log_level) {
      continue;
    }

    const char *prefix = "[INFO]";
    if (logs[i].level == LOG_WARNING)
      prefix = "[WARN]";
    if (logs[i].level == LOG_ERROR)
      prefix = "[ERR ]";

    // Print the log and move to the next row
    mvwprintw(log_win, display_row, 0, "%s %s", prefix, logs[i].msg);
    display_row++;

    // Stop if we run out of window space
    if (display_row > LOG_CAPACITY)
      break;
  }

  wrefresh(log_win);
}

void log_resize() {
  if (!log_win)
    return;

  int height = LOG_CAPACITY + 1;
  wresize(log_win, height, COLS);
  mvwin(log_win, LINES - height, 0);
}

void log_cleanup() {
  if (log_win) {
    delwin(log_win);
  }
}
