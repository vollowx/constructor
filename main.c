#include <time.h>

#include "fcp.h"
#include "info.h"
#include "log.h"
#include "options.h"

#define FPS 60

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
  GameInfo info = {
      .cur_state = (GameState)-1,
      .next_state = STATE_MAIN_MENU,
      .cur_slot = 0,
  };

  initscr();
  cbreak();
  noecho();
  keypad(stdscr, TRUE);
  nodelay(stdscr, TRUE);
  curs_set(0);

  if (!has_colors()) {
    printw("Your terminal does not support color. Press any key to quit.");
    getch();
    return 1;
  }

  start_color();
  use_default_colors();
  fcp_init();

  options_load();

  struct timespec last_frame, current_frame;
  clock_gettime(CLOCK_MONOTONIC, &last_frame);

#define X(name) name##_init(&info);
  AM_MAP(X)
#undef X

  while (info.next_state != STATE_QUIT) {
    clock_gettime(CLOCK_MONOTONIC, &current_frame);
    double dt = (current_frame.tv_sec - last_frame.tv_sec) +
                (current_frame.tv_nsec - last_frame.tv_nsec) / 1e9;
    last_frame = current_frame;

    if (info.next_state != info.cur_state) {
      if (pm) {
        pm->deinit();
      }

      info.cur_state = info.next_state;
      update_activity_ptr(info.cur_state);

      pm->init(&info);
    }

    info.ch = getch();
    if (info.ch == KEY_RESIZE) {
      erase();

      if (pm)
        pm->resize(&info);
#define X(name) name##_resize(&info);
      AM_MAP(X)
#undef X

      refresh();
    } else {
      if (pm)
        pm->input(&info);
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
