#ifndef TUI_CONTEXT_H
#define TUI_CONTEXT_H

#include <ncurses.h>

#include "core/context.h"

typedef struct CwTui CwTui;

#define OVERLAY_MAP(X) X(log)

// X(state, name)
#define SCREEN_MAP(X)                                                          \
    X(MAIN_MENU, main_menu)                                                    \
    X(SAVES, saves)                                                            \
    X(GAMEPLAY, gameplay)                                                      \
    X(OPTIONS, options)                                                        \
    X(ABOUT, about)

typedef struct {
    void (*init)(CwTui *ctx);
    void (*frame)(double dt);
    void (*resize)(CwTui *ctx);
    void (*deinit)(void);
} Overlay;

typedef struct {
    void (*init)(CwTui *ctx);
    void (*input)(CwTui *ctx);
    void (*frame)(double dt);
    void (*resize)(CwTui *ctx);
    void (*deinit)(void);
} Screen;

#define X(name)                                                                \
    void name##_init(CwTui *ctx);                                              \
    void name##_frame(double dt);                                              \
    void name##_resize(CwTui *ctx);                                            \
    void name##_deinit();                                                      \
    extern Overlay overlay_##name;
OVERLAY_MAP(X)
#undef X

#define X(state, name)                                                         \
    void name##_init(CwTui *ctx);                                              \
    void name##_input(CwTui *ctx);                                             \
    void name##_frame(double dt);                                              \
    void name##_resize(CwTui *ctx);                                            \
    void name##_deinit();                                                      \
    extern Screen screen_##name;
SCREEN_MAP(X)
#undef X

typedef enum {
#define X(state, name) TUI_STATE_##state,
    SCREEN_MAP(X)
#undef X
        TUI_STATE_QUIT,
    __tui_state_count,
} CwTuiState;

// charwild TUI context
struct CwTui {
    int ch;
    CwTuiState cur_state;
    CwTuiState next_state;
    Screen *cur_screen;

    Cw *core;
};

#endif // INFO_H
