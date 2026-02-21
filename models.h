#ifndef MODELS_H
#define MODELS_H

#include <ncurses.h>

typedef struct {
  void (*init)();
  void (*input)(int ch);
  void (*render)();
  void (*resize)();
  void (*cleanup)();
} Model;

#define AM_MAP(X) X(log)

// X(state, name)
#define PM_MAP(X)                                                              \
  X(MAIN_MENU, main_menu)                                                      \
  X(SAVES, saves)                                                              \
  X(GAMEPLAY, gameplay)                                                        \
  X(OPTIONS, options)

// Implementation filename e.g.
// am_log.c   - Always-on model
// pm_saves.c - Primary model

#define X(name)                                                                \
  void name##_init();                                                          \
  void name##_input(int ch);                                                   \
  void name##_render();                                                        \
  void name##_resize();                                                        \
  void name##_cleanup();                                                       \
  extern Model model_##name;
AM_MAP(X)
#undef X

#define X(state, name)                                                         \
  void name##_init();                                                          \
  void name##_input(int ch);                                                   \
  void name##_render();                                                        \
  void name##_resize();                                                        \
  void name##_cleanup();                                                       \
  extern Model model_##name;
PM_MAP(X)
#undef X

typedef enum {
#define X(state, name) STATE_##state,
  PM_MAP(X)
#undef X
      STATE_COUNT,
  STATE_QUIT
} GameState;

extern GameState current_state;
extern GameState next_state;

#endif
