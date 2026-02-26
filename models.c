#include "models.h"

#define X(state, name)                                                         \
  Model model_##name = {name##_init, name##_input, name##_frame,               \
                        name##_resize, name##_deinit};
PM_MAP(X)
#undef X
