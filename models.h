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

// X(state, name)
#define MODEL_MAP(X)                                                           \
  X(UNREACHABLE, log)                                                          \
  X(MAIN_MENU, main_menu)                                                      \
  X(GAMEPLAY, gameplay)                                                        \
  X(OPTIONS, options)

#define DEFINE_MODEL(name)                                                     \
  Model model_##name = {name##_init, name##_input, name##_render,              \
                        name##_resize, name##_cleanup}

#define X(state, name)                                                         \
  void name##_init();                                                          \
  void name##_input(int ch);                                                   \
  void name##_render();                                                        \
  void name##_resize();                                                        \
  void name##_cleanup();                                                       \
  extern Model model_##name;
MODEL_MAP(X)
#undef X

#endif
