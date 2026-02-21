#include <assert.h>

#include "log.h"
#include "models.h"
#include "options.h"

WINDOW *log_win = NULL;

void log_init() {
  int height = LOG_CAPACITY + 1;
  log_win = newwin(height, COLS, LINES - height, 0);
}

void log_input(int ch) {
  (void)ch;
  assert(0 && "UNREACHABLE");
}

void log_render() {
  if (!log_win || !current_options.show_log)
    return;

  werase(log_win);
  mvwhline(log_win, 0, 0, ACS_HLINE, COLS);

  int display_row = 1;

  for (int i = 0; i < log_count; i++) {
    if ((int)logs[i].level < current_options.log_level) {
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

DEFINE_MODEL(log);
