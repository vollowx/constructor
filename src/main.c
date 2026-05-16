#include <time.h>

#include "core/log.h"
#include "core/options.h"
#include "ui/fcp.h"
#include "ui/tui_context.h"

#define CW_FPS 60

int main(int argc, char *argv[]) {
    Cw core_ctx = {
        .cur_slot = 0,
    };
    CwTui ctx = {
        .cur_state = (CwTuiState)-1,
        .next_state = TUI_STATE_MAIN_MENU,

        .core = &core_ctx,
    };

    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    nodelay(stdscr, TRUE);
    curs_set(0);

    if (!has_colors()) {
        printw("Your terminal does not support color. Press any key to quit.");
        getch();
        return 1;
    }

    start_color();
    use_default_colors();
    fcp_init();

    options_load();

    struct timespec last_frame, current_frame;
    clock_gettime(CLOCK_MONOTONIC, &last_frame);

#define X(name) name##_init(&ctx);
    OVERLAY_MAP(X)
#undef X

    while (ctx.next_state != TUI_STATE_QUIT) {
        clock_gettime(CLOCK_MONOTONIC, &current_frame);
        double dt = (current_frame.tv_sec - last_frame.tv_sec) +
                    (current_frame.tv_nsec - last_frame.tv_nsec) / 1e9;
        last_frame = current_frame;

        if (ctx.next_state != ctx.cur_state) {
            if (ctx.cur_screen) {
                ctx.cur_screen->deinit();
            }

            ctx.cur_state = ctx.next_state;
            switch (ctx.cur_state) {
#define X(state, suffix)                                                       \
            case TUI_STATE_##state:                                            \
                ctx.cur_screen = &screen_##suffix;                             \
                break;
            SCREEN_MAP(X)
#undef X
            default:
                ctx.cur_screen = NULL;
                break;
            }

            ctx.cur_screen->init(&ctx);
        }

        ctx.ch = getch();
        if (ctx.ch == KEY_RESIZE) {
            erase();

            if (ctx.cur_screen)
                ctx.cur_screen->resize(&ctx);
#define X(name) name##_resize(&ctx);
                OVERLAY_MAP(X)
#undef X

            refresh();
        } else {
            if (ctx.cur_screen)
                ctx.cur_screen->input(&ctx);
        }

        if (ctx.cur_screen)
            ctx.cur_screen->frame(dt);
#define X(name) name##_frame(dt);
            OVERLAY_MAP(X)
#undef X

        doupdate();

        napms(1000 / CW_FPS);
    }

    if (ctx.cur_screen)
        ctx.cur_screen->deinit();
#define X(name) name##_deinit();
    OVERLAY_MAP(X)
#undef X

    endwin();

    free_logs();

    return 0;
}
