#ifndef INFO_H
#define INFO_H

#include <ncurses.h>

#define OVERLAY_MAP(X) X(log)

// X(state, name)
#define SCREEN_MAP(X)                                                          \
    X(MAIN_MENU, main_menu)                                                    \
    X(SAVES, saves)                                                            \
    X(GAMEPLAY, gameplay)                                                      \
    X(OPTIONS, options)                                                        \
    X(ABOUT, about)

typedef struct AppContext AppContext;

typedef struct {
    void (*init)(AppContext *ctx);
    void (*frame)(double dt);
    void (*resize)(AppContext *ctx);
    void (*deinit)();
} Overlay;

typedef struct {
    void (*init)(AppContext *ctx);
    void (*input)(AppContext *ctx);
    void (*frame)(double dt);
    void (*resize)(AppContext *ctx);
    void (*deinit)();
} Screen;

typedef enum {
#define X(state, name) APP_STATE_##state,
    SCREEN_MAP(X)
#undef X
        APP_STATE_QUIT,
    _app_state_count,
} AppState;

struct AppContext {
    AppState cur_state;
    AppState next_state;
    Screen *cur_screen;
    size_t cur_slot;
    int ch;
};

#define X(name)                                                                \
    void name##_init(AppContext *ctx);                                         \
    void name##_frame(double dt);                                              \
    void name##_resize(AppContext *ctx);                                       \
    void name##_deinit();                                                      \
    extern Overlay overlay_##name;
OVERLAY_MAP(X)
#undef X

#define X(state, name)                                                         \
    void name##_init(AppContext *ctx);                                         \
    void name##_input(AppContext *ctx);                                        \
    void name##_frame(double dt);                                              \
    void name##_resize(AppContext *ctx);                                       \
    void name##_deinit();                                                      \
    extern Screen screen_##name;
SCREEN_MAP(X)
#undef X

#endif // INFO_H
