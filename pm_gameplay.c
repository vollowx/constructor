#include "log.h"
#include "models.h"

void gameplay_init() { info("[model] model_gameplay initializing"); }

void gameplay_input(int ch) {
  if (ch == 'm') {
    next_state = STATE_MAIN_MENU;
  }
}

void gameplay_render() {
  mvprintw(0, 0, "Game Started! Press 'm' for Menu.");
  mvprintw(2, 0, "Rendering Forager World... (Character: @)");
  refresh();
}

void gameplay_resize() {}

void gameplay_cleanup() {}
