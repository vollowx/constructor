#include "log.h"
#include "models.h"
#include "states.h"

void gameplay_init() {}

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

DEFINE_MODEL(gameplay);
