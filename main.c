#include <time.h>

#include "fcp.h"
#include "log.h"
#include "models.h"
#include "options.h"

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

  int ch;

#define X(name) name##_init();
  AM_MAP(X)
#undef X

  while (next_state != STATE_QUIT) {
    if (next_state != current_state) {
      if (pm) {
        pm->deinit();
      }

      current_state = next_state;
      update_activity_ptr(current_state);

      pm->init();
    }

    ch = getch();
    if (ch != ERR) {
      if (ch == KEY_RESIZE) {
        erase();

#define X(name) name##_resize();
        AM_MAP(X)
#undef X

        pm->resize();

        refresh();
      } else {
        pm->input(ch);
      }
    }

    pm->frame();

#define X(name) name##_frame();
    AM_MAP(X)
#undef X

    doupdate();

    napms(16);
  }

#define X(name) name##_deinit();
  AM_MAP(X)
#undef X

  if (pm)
    pm->deinit();

  endwin();

  free_logs();

  return 0;
}
