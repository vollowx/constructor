#include <menu.h>

#include "ui/app_state.h"
#include "ui/fcp.h"
#include "core/helpers.h"
#include "core/log.h"

#define MAIN_MENU_HEIGHT 8
#define MAIN_MENU_WIDTH 36
#define MAIN_MENU_N_ITEMS 4

ITEM **m_items;
MENU *m_menu;
WINDOW *m_win;

void main_menu_init(AppContext *ctx) {
    info("[model] major = main_menu");

    char *labels[] = {"Start Game", "Options", "About",
                      "Quit                         ", (char *)NULL};

    m_items = (ITEM **)calloc(MAIN_MENU_N_ITEMS + 1, sizeof(ITEM *));
    for (int i = 0; i < MAIN_MENU_N_ITEMS; ++i)
        m_items[i] = new_item(labels[i], "");

    m_menu = new_menu(m_items);
    m_win =
        newwin(MAIN_MENU_HEIGHT, MAIN_MENU_WIDTH,
               (LINES - MAIN_MENU_HEIGHT) / 2, (COLS - MAIN_MENU_WIDTH) / 2);
    keypad(m_win, TRUE);

    set_menu_win(m_menu, m_win);
    set_menu_sub(
        m_menu, derwin(m_win, MAIN_MENU_HEIGHT - 4, MAIN_MENU_WIDTH - 4, 2, 1));
    set_menu_mark(m_menu, " > ");
    set_menu_fore(m_menu,
                  COLOR_PAIR(fcp_get(COLOR_BLUE, -1)) | A_BOLD | A_REVERSE);
    post_menu(m_menu);
}

void main_menu_deinit() {
    werase(m_win);
    wnoutrefresh(m_win);
    free_menu_ctx(m_win, m_menu, m_items, MAIN_MENU_N_ITEMS, false);
}

void main_menu_input(AppContext *ctx) {
    switch (ctx->ch) {
    case KEY_DOWN:
    case 'j':
        menu_driver(m_menu, REQ_DOWN_ITEM);
        break;
    case KEY_UP:
    case 'k':
        menu_driver(m_menu, REQ_UP_ITEM);
        break;
    case 'q':
        ctx->next_state = APP_STATE_QUIT;
        break;
    case 10: {
        int index = item_index(current_item(m_menu));
        switch (index) {
        case 0:
            ctx->next_state = APP_STATE_SAVES;
            break;
        case 1:
            ctx->next_state = APP_STATE_OPTIONS;
            break;
        case 2:
            ctx->next_state = APP_STATE_ABOUT;
            break;
        case 3:
            ctx->next_state = APP_STATE_QUIT;
            break;
        }
        break;
    }
    }
}

void main_menu_frame(double dt) {
    UNUSED(dt);
    draw_win_frame(m_win, "Constructor", COLOR_BLUE);
    wnoutrefresh(m_win);
}

void main_menu_resize(AppContext *ctx) {
    mvwin(m_win, (LINES - MAIN_MENU_HEIGHT) / 2, (COLS - MAIN_MENU_WIDTH) / 2);
}
