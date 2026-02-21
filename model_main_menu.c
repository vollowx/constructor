#include <stdlib.h>

#include <menu.h>

#include "log.h"
#include "models.h"
#include "states.h"

#define MAIN_MENU_HEIGHT 4
#define MAIN_MENU_WIDTH 32

ITEM **m_items;
MENU *m_menu;
WINDOW *m_win;

void main_menu_init() {
  char *labels[] = {"Start Game", "Options", "About",
                    "Quit                         ", (char *)NULL};
  int n_choices = 4;

  m_items = (ITEM **)calloc(n_choices + 1, sizeof(ITEM *));
  for (int i = 0; i < n_choices; ++i)
    m_items[i] = new_item(labels[i], "");

  m_menu = new_menu(m_items);
  m_win = newwin(MAIN_MENU_HEIGHT + 4, MAIN_MENU_WIDTH + 4,
                 (LINES + 2 - MAIN_MENU_HEIGHT) / 2,
                 (COLS + 2 - MAIN_MENU_WIDTH) / 2);
  keypad(m_win, TRUE);

  set_menu_win(m_menu, m_win);
  set_menu_sub(m_menu, derwin(m_win, MAIN_MENU_HEIGHT, MAIN_MENU_WIDTH, 2, 1));
  set_menu_mark(m_menu, " > ");
  box(m_win, 0, 0);
  mvwprintw(m_win, 0, 3, " Constructor ");
  post_menu(m_menu);

  refresh();
}

void main_menu_input(int ch) {
  static int counter = 0;

  switch (ch) {
  case KEY_DOWN:
    menu_driver(m_menu, REQ_DOWN_ITEM);
    break;
  case KEY_UP:
    menu_driver(m_menu, REQ_UP_ITEM);
    break;
  case 't':
    warn("test log no. %d", counter++);
    break;
  case 10: {
    int index = item_index(current_item(m_menu));
    switch (index) {
    case 0:
      next_state = STATE_SAVES;
      break;
    case 1:
      next_state = STATE_OPTIONS;
      break;
    case 2:
      // next_state = STATE_ABOUT;
      break;
    case 3:
      next_state = STATE_QUIT;
      break;
    }
    break;
  }
  }
}

void main_menu_render() { wrefresh(m_win); }

void main_menu_resize() {
  mvwin(m_win, (LINES + 2 - MAIN_MENU_HEIGHT) / 2,
        (COLS + 2 - MAIN_MENU_WIDTH) / 2);
}

void main_menu_cleanup() {
  unpost_menu(m_menu);
  free_menu(m_menu);
  for (int i = 0; i < 3; ++i)
    free_item(m_items[i]);
  free(m_items);
  delwin(m_win);
}

DEFINE_MODEL(main_menu);
