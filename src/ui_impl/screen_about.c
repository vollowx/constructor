#include "core/helpers.h"
#include "core/log.h"
#include "ui/tui_context.h"

#define ABOUT_HEIGHT 9
#define ABOUT_WIDTH 60

WINDOW *a_win;

void about_init(CwTui *ctx) {
    info("[model] screen = about");

    a_win = newwin(ABOUT_HEIGHT, ABOUT_WIDTH, (LINES - ABOUT_HEIGHT) / 2,
                   (COLS - ABOUT_WIDTH) / 2);

    keypad(a_win, TRUE);
}

void about_deinit(void) {
    if (!a_win)
        return;

    werase(a_win);
    wnoutrefresh(a_win);
    delwin(a_win);
    a_win = NULL;
}

void about_input(CwTui *ctx) {
    switch (ctx->ch) {
    case 'q':
        ctx->next_state = TUI_STATE_MAIN_MENU;
        break;
    }
}

void about_frame(double dt) {
    UNUSED(dt);

    if (!a_win)
        return;

    draw_win_frame(a_win, "About", COLOR_BLUE);
    mvwprintw(a_win, 2, 4, "charwild v%d.%d.%d", CW_VERSION_MAJOR,
              CW_VERSION_MINOR, CW_VERSION_PATCH);
    mvwprintw(a_win, 3, 4, "    developed by Lucas X. Zhao");
    mvwprintw(a_win, 4, 4, "    licensed under Apache-2.0");
    mvwprintw(a_win, 6, 4, "charwild is a survival sandbox game in TUI.");
    wnoutrefresh(a_win);
}

void about_resize(CwTui *ctx) {
    if (!a_win)
        return;

    mvwin(a_win, (LINES - ABOUT_HEIGHT) / 2, (COLS - ABOUT_WIDTH) / 2);
}
