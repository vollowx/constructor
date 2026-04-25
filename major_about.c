#include <menu.h>

#include "fcp.h"
#include "helpers.h"
#include "log.h"
#include "models.h"

#define ABOUT_HEIGHT 28
#define ABOUT_WIDTH 60

WINDOW *a_win;

void about_init() {
  info("[model] major = about");

  a_win = newwin(ABOUT_HEIGHT, ABOUT_WIDTH, (LINES - ABOUT_HEIGHT) / 2,
                 (COLS - ABOUT_WIDTH) / 2);

  keypad(a_win, TRUE);
}

void about_deinit() {
  werase(a_win);
  wnoutrefresh(a_win);
}

void about_input(int ch) {
  switch (ch) {
  case 'q':
    next_state = STATE_MAIN_MENU;
    break;
  }
}

void about_frame(double dt) {
  UNUSED(dt);

  if (!a_win)
    return;

  draw_win_frame(a_win, "About", COLOR_BLUE);
  wnoutrefresh(a_win);
}

void about_resize() {
  if (!a_win)
    return;

  mvwin(a_win, (LINES - ABOUT_HEIGHT) / 2, (COLS - ABOUT_WIDTH) / 2);
}
