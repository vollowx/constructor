#ifndef ACTIVITIES_H
#define ACTIVITIES_H

#include <ncurses.h>

typedef struct {
  void (*init)();
  void (*input)(int ch);
  void (*render)();
  void (*resize)();
  void (*cleanup)();
} Activity;

// X(state, name)
#define ACTIVITY_MAP(X)                                                        \
  X(MAIN_MENU, main_menu)                                                      \
  X(GAMEPLAY, gameplay)                                                        \
  X(OPTIONS, options)

#define DEFINE_ACTIVITY(name)                                                  \
  Activity activity_##name = {name##_init, name##_input, name##_render,        \
                              name##_resize, name##_cleanup}

#define X(state, name)                                                         \
  void name##_init();                                                          \
  void name##_input(int ch);                                                   \
  void name##_render();                                                        \
  void name##_resize();                                                        \
  void name##_cleanup();                                                       \
  extern Activity activity_##name;
ACTIVITY_MAP(X)
#undef X

#endif
