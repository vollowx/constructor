#include <time.h>

#include "fcp.h"
#include "log.h"
#include "models.h"
#include "options.h"

#define FPS 60

int current_slot = 0;
GameState next_state = STATE_MAIN_MENU;
GameState current_state = (GameState)-1;
// Current primary model
Model *pm = NULL;

void update_activity_ptr(GameState state) {
  switch (state) {
#define X(_state, suffix)                                                      \
  case STATE_##_state:                                                         \
    pm = &model_##suffix;                                                      \
    break;
    PM_MAP(X)
#undef X
  default:
    pm = NULL;
    break;
  }
}

int main() {
  initscr();
  cbreak();
  noecho();
  keypad(stdscr, TRUE);
  nodelay(stdscr, TRUE);
  curs_set(0);

  start_color();
  fcp_init();

  options_load();

  struct timespec last_frame, current_frame;
  clock_gettime(CLOCK_MONOTONIC, &last_frame);

  int ch;

#define X(name) name##_init();
  AM_MAP(X)
#undef X

  while (next_state != STATE_QUIT) {
    clock_gettime(CLOCK_MONOTONIC, &current_frame);
    double dt = (current_frame.tv_sec - last_frame.tv_sec) +
                (current_frame.tv_nsec - last_frame.tv_nsec) / 1e9;
    last_frame = current_frame;

    if (next_state != current_state) {
      if (pm) {
        pm->deinit();
      }

      current_state = next_state;
      update_activity_ptr(current_state);

      pm->init();
    }

    ch = getch();
    if (ch == KEY_RESIZE) {
      erase();

      if (pm)
        pm->resize();
#define X(name) name##_resize();
      AM_MAP(X)
#undef X

      refresh();
    } else {
      if (pm)
        pm->input(ch);
    }

    if (pm)
      pm->frame(dt);
#define X(name) name##_frame(dt);
    AM_MAP(X)
#undef X

    doupdate();

    napms(1000 / FPS);
  }

  if (pm)
    pm->deinit();
#define X(name) name##_deinit();
  AM_MAP(X)
#undef X

  endwin();

  free_logs();

  return 0;
}
