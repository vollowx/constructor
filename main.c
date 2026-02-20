#include "log.h"
#include "models.h"
#include "options.h"
#include "states.h"

GameState next_state = STATE_MAIN_MENU;
GameState current_state = (GameState)-1;
Model *current_model = NULL;

void update_activity_ptr(GameState state) {
  switch (state) {
#define X(_state, suffix)                                                      \
  case STATE_##_state:                                                         \
    current_model = &model_##suffix;                                           \
    break;
    MODEL_MAP(X)
#undef X
  default:
    current_model = NULL;
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
      if (current_model) {
        erase();
        current_model->cleanup();
      }

      current_state = next_state;
      update_activity_ptr(current_state);

      if (current_model) {
        current_model->init();
        current_model->render();
      }
    }

    timeout(16);

    if (current_model) {
      ch = getch();

      if (ch != ERR) {
        if (ch == KEY_RESIZE) {
          erase();
          current_model->resize();
          log_resize();
          refresh();
        } else {
          current_model->input(ch);
        }
      }

      current_model->render();
    }

    log_render();
  }

  log_cleanup();
  if (current_model)
    current_model->cleanup();
  endwin();

  return 0;
}
