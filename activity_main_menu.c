#include <menu.h>
#include <stdlib.h>

#include "activities.h"
#include "log.h"
#include "states.h"

#define MAIN_MENU_HEIGHT 4
#define MAIN_MENU_WIDTH 32

extern int next_state;

ITEM **my_items;
MENU *my_menu;
WINDOW *my_menu_win;

void main_menu_init() {
  char *labels[] = {"Start New Game", "Saves", "Options",
                    "Quit                         ", (char *)NULL};
  int n_choices = 4;

  my_items = (ITEM **)calloc(n_choices + 1, sizeof(ITEM *));
  for (int i = 0; i < n_choices; ++i)
    my_items[i] = new_item(labels[i], "");

  my_menu = new_menu(my_items);
  my_menu_win = newwin(MAIN_MENU_HEIGHT + 4, MAIN_MENU_WIDTH + 4,
                       (LINES + 4 - MAIN_MENU_HEIGHT) / 2,
                       (COLS + 4 - MAIN_MENU_WIDTH) / 2);
  keypad(my_menu_win, TRUE);

  set_menu_win(my_menu, my_menu_win);
  set_menu_sub(my_menu,
               derwin(my_menu_win, MAIN_MENU_HEIGHT, MAIN_MENU_WIDTH, 2, 1));
  set_menu_mark(my_menu, " > ");
  box(my_menu_win, 0, 0);
  mvwprintw(my_menu_win, 0, 3, " Constructor ");
  post_menu(my_menu);

  refresh();
}

void main_menu_input(int ch) {
  switch (ch) {
  case KEY_DOWN:
    menu_driver(my_menu, REQ_DOWN_ITEM);
    break;
  case KEY_UP:
    menu_driver(my_menu, REQ_UP_ITEM);
    break;
  case 10: {
    int index = item_index(current_item(my_menu));
    switch (index) {
    case 0:
      next_state = STATE_GAMEPLAY;
      break;
    case 1:
      next_state = STATE_SAVES;
      break;
    case 2:
      next_state = STATE_OPTIONS;
      break;
    case 3:
      next_state = STATE_QUIT;
      break;
    }
    break;
  }
  }
}

void main_menu_render() { wrefresh(my_menu_win); }

void main_menu_resize() {
  mvwin(my_menu_win, (LINES + 4 - MAIN_MENU_HEIGHT) / 2,
        (COLS + 4 - MAIN_MENU_WIDTH) / 2);
}

void main_menu_cleanup() {
  unpost_menu(my_menu);
  free_menu(my_menu);
  for (int i = 0; i < 3; ++i)
    free_item(my_items[i]);
  free(my_items);
  delwin(my_menu_win);
}

DEFINE_ACTIVITY(main_menu);
