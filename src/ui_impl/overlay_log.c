#include "core/helpers.h"
#include "core/log.h"
#include "core/options.h"
#include "ui/fcp.h"
#include "ui/tui_context.h"

WINDOW *l_win = NULL;
static short log_cp[3];

void log_init(CwTui *ctx) {
    info("[model] overlay += log");

    log_cp[LOG_INFO] = fcp_get(COLOR_BLUE, -1);
    log_cp[LOG_WARNING] = fcp_get(COLOR_YELLOW, -1);
    log_cp[LOG_ERROR] = fcp_get(COLOR_RED, -1);

    int height = LOG_UI_CAPACITY + 1;
    l_win = newwin(height, COLS, LINES - height, 0);
}

void log_deinit(void) {
    werase(l_win);
    wnoutrefresh(l_win);
    if (l_win) {
        delwin(l_win);
    }
}

void log_frame(double dt) {
    UNUSED(dt);

    if (!l_win)
        return;

    static bool was_showing = false;

    if (!current_options.show_log) {
        if (was_showing) {
            werase(l_win);
            wnoutrefresh(l_win);
            was_showing = false;
        }
        return;
    }

    was_showing = true;

    werase(l_win);
    wattron(l_win, COLOR_PAIR(fcp_get(COLOR_MAGENTA, -1)));
    mvwhline(l_win, 0, 0, ACS_HLINE, COLS);
    wattroff(l_win, COLOR_PAIR(fcp_get(COLOR_MAGENTA, -1)));

    size_t line = LOG_UI_CAPACITY;
    size_t i = logs.count;

    while (i > 0 && line > 0) {
        const Log *log = &logs.items[--i];

        if ((int)log->level >= current_options.log_level) {
            short cp = log_cp[log->level];
            wattron(l_win, COLOR_PAIR(cp));
            mvwprintw(l_win, line--, 0, "%s", log->msg);
            wattroff(l_win, COLOR_PAIR(cp));
        }
    }

    wnoutrefresh(l_win);
}

void log_resize(CwTui *ctx) {
    if (!l_win)
        return;

    int height = LOG_UI_CAPACITY + 1;
    wresize(l_win, height, COLS);
    mvwin(l_win, LINES - height, 0);
}
