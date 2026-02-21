#include <assert.h>
#include <ncurses.h>

#include "helpers.h"
#include "log.h"
#include "models.h"
#include "options.h"

WINDOW *l_win = NULL;

void log_init() {
  int height = LOG_UI_CAPACITY + 1;
  l_win = newwin(height, COLS, LINES - height, 0);
}

void log_input(int ch) {
  UNUSED(ch);
  assert(0 && "Unreachable");
}

void log_render() {
  if (!l_win || !current_options.show_log)
    return;

  werase(l_win);
  mvwhline(l_win, 0, 0, ACS_HLINE, COLS);

  size_t line = LOG_UI_CAPACITY;
  size_t i = logs.count;

  while (i > 0 && line > 0) {
    const Log *log = &logs.items[--i];

    if ((int)log->level >= current_options.log_level) {
      int color = (log->level == LOG_ERROR)     ? 3
                  : (log->level == LOG_WARNING) ? 2
                                                : 1;

      wattron(l_win, COLOR_PAIR(color));
      mvwprintw(l_win, line--, 0, "%s", log->msg);
      wattroff(l_win, COLOR_PAIR(color));
    }
  }

  wrefresh(l_win);
}

void log_resize() {
  if (!l_win)
    return;

  int height = LOG_UI_CAPACITY + 1;
  wresize(l_win, height, COLS);
  mvwin(l_win, LINES - height, 0);
}

void log_cleanup() {
  if (l_win) {
    delwin(l_win);
  }
}
