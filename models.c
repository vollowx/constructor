#include "models.h"

#define X(state, name)                                                         \
  Model model_##name = {name##_init, name##_input, name##_render,              \
                        name##_resize, name##_cleanup};
PM_MAP(X)
#undef X
