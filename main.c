#include "activities.h"
#include "log.h"
#include "options.h"
#include "states.h"

GameState next_state = STATE_MAIN_MENU;
GameState current_state = (GameState)-1;
Activity *current_activity = NULL;

void update_activity_ptr(GameState state) {
  switch (state) {
#define X(enum_part, suffix)                                                   \
  case STATE_##enum_part:                                                      \
    current_activity = &activity_##suffix;                                     \
    break;
    ACTIVITY_MAP(X)
#undef X
  default:
    current_activity = NULL;
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

  log_init();
  log_add(LOG_INFO, "TUI initialized");

  options_load();

  int ch;

  while (next_state != STATE_QUIT) {
    if (next_state != current_state) {
      if (current_activity)
        current_activity->cleanup();

      current_state = next_state;
      update_activity_ptr(current_state);

      if (current_activity) {
        current_activity->init();
        current_activity->render();
      }
    }

    timeout(16);

    if (current_activity) {
      ch = getch();

      if (ch != ERR) {
        if (ch == KEY_RESIZE) {
          erase();
          current_activity->resize();
          log_resize();
          refresh();
        } else {
          current_activity->input(ch);
        }
        current_activity->render();
      }
    }

    log_render();
  }

  log_cleanup();
  if (current_activity)
    current_activity->cleanup();
  endwin();

  return 0;
}
