#include "app_state.h"

#define X(name)                                                                \
    Overlay overlay_##name = {name##_init, name##_frame, name##_resize,        \
                              name##_deinit};
OVERLAY_MAP(X)
#undef X

#define X(state, name)                                                         \
    Screen screen_##name = {name##_init, name##_input, name##_frame,           \
                            name##_resize, name##_deinit};
SCREEN_MAP(X)
#undef X
