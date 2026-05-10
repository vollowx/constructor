#ifndef INFO_H
#define INFO_H

#include <ncurses.h>

#define AM_MAP(X) X(log)

// X(state, name)
#define PM_MAP(X)                                                              \
  X(MAIN_MENU, main_menu)                                                      \
  X(SAVES, saves)                                                              \
  X(GAMEPLAY, gameplay)                                                        \
  X(OPTIONS, options)                                                          \
  X(ABOUT, about)

typedef enum {
#define X(state, name) STATE_##state,
  PM_MAP(X)
#undef X
      STATE_COUNT,
  STATE_QUIT
} GameState;

typedef struct {
  GameState cur_state;
  GameState next_state;
  size_t cur_slot;
  int ch;
} GameInfo;

typedef struct {
  void (*init)(GameInfo *info);
  void (*input)(GameInfo *info);
  void (*frame)(double dt);
  void (*resize)(GameInfo *info);
  void (*deinit)();
} Model;

// Implementation filename e.g.
// minor_log.c   - Always-on model
// major_saves.c - Primary model

// TODO: Consider remove ..._input() functions for always-on models
#define X(name)                                                                \
  void name##_init(GameInfo *info);                                            \
  void name##_input(GameInfo *info);                                           \
  void name##_frame(double dt);                                                \
  void name##_resize(GameInfo *info);                                          \
  void name##_deinit();                                                        \
  extern Model model_##name;
AM_MAP(X)
#undef X

#define X(state, name)                                                         \
  void name##_init(GameInfo *info);                                            \
  void name##_input(GameInfo *info);                                           \
  void name##_frame(double dt);                                                \
  void name##_resize(GameInfo *info);                                          \
  void name##_deinit();                                                        \
  extern Model model_##name;
PM_MAP(X)
#undef X

#endif // INFO_H
