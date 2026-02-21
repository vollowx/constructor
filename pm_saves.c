#include <string.h>

#include <menu.h>

#include "helpers.h"
#include "log.h"
#include "models.h"
#include "save.h"

// menu height = 3, padding = 1 * 2, border = 1 * 2
#define SAVES_HEIGHT 7
#define SAVES_WIDTH 48
#define PREVIEW_HEIGHT 24
#define PREVIEW_WIDTH 56

static ITEM **s_items = NULL;
static MENU *s_menu = NULL;
static WINDOW *s_win = NULL;
static WINDOW *s_pre = NULL;
static SavePreview previews[3];

void rebuild_saves_menu() {
  if (s_menu) {
    unpost_menu(s_menu);
    free_menu(s_menu);
  }
  if (s_items) {
    for (int i = 0; i < 3; i++) {
      free((void *)item_name(s_items[i]));
      free_item(s_items[i]);
    }
    free(s_items);
    s_items = NULL;
  }

  s_items = (ITEM **)calloc(4, sizeof(ITEM *));

  for (int i = 0; i < 3; i++) {
    char label[SAVES_WIDTH - 6];

    previews[i] = get_slot_preview(i);
    snprintf(label, sizeof(label), "Slot %d: %33s", i,
             previews[i].exists ? previews[i].header.player_name : "<Empty>");

    s_items[i] = new_item(strdup(label), "");
  }
  s_items[3] = NULL;

  s_menu = new_menu(s_items);
  set_menu_win(s_menu, s_win);
  set_menu_sub(s_menu, derwin(s_win, SAVES_HEIGHT - 4, SAVES_WIDTH - 4, 2, 1));
  set_menu_mark(s_menu, " > ");

  post_menu(s_menu);
}

void saves_init() {
  info("primary model switched to saves");

  //                          gap
  int total_w = SAVES_WIDTH + 1 + PREVIEW_WIDTH;
  int start_x = (COLS - total_w) / 2;
  int start_y = (LINES - PREVIEW_HEIGHT) / 2;

  s_win = newwin(SAVES_HEIGHT, SAVES_WIDTH, start_y, start_x);
  s_pre =
      newwin(PREVIEW_HEIGHT, PREVIEW_WIDTH, start_y, start_x + SAVES_WIDTH + 1);

  keypad(s_win, TRUE);
  rebuild_saves_menu();
}

void saves_input(int ch) {
  int idx = item_index(current_item(s_menu));

  switch (ch) {
  case KEY_DOWN:
    menu_driver(s_menu, REQ_DOWN_ITEM);
    break;
  case KEY_UP:
    menu_driver(s_menu, REQ_UP_ITEM);
    break;

  case 'q':
    next_state = STATE_MAIN_MENU;
    break;

  case 10:
    if (previews[idx].exists) {
      next_state = STATE_GAMEPLAY;
    } else {
      next_state = STATE_GAMEPLAY;
    }

    break;

  case 'x':
    // current_slot = ...
    // next_state = STATE_SAVE_DELETE
    break;
  case 'r':
    info("rename requested for slot %d", idx);
    if (previews[idx].exists) {
      // current_slot = ...
      // next_state = STATE_SAVE_RENAME
    } else {
      info("Slot %d does not exist", idx);
    }
    break;
  case 27: // ESC
    next_state = STATE_MAIN_MENU;
    break;
  }
}

void saves_render() {
  if (!s_win || !s_pre)
    return;

  box(s_win, 0, 0);
  mvwprintw(s_win, 0, 2, " Select Save ");
  wrefresh(s_win);

  werase(s_pre);
  box(s_pre, 0, 0);
  mvwprintw(s_pre, 0, 2, " Preview ");

  int idx = item_index(current_item(s_menu));
  if (previews[idx].exists) {
    wattron(s_pre, COLOR_PAIR(1)); // Using INFO color for name
    mvwprintw(s_pre, 2, 2, "PLAYER: %s", previews[idx].header.player_name);
    wattroff(s_pre, COLOR_PAIR(1));

    mvwprintw(s_pre, 4, 2, "Version:  %d", previews[idx].header.version);
    mvwprintw(s_pre, 5, 2, "Location: %s", "The Wilderness"); // Example
  } else {
    wattron(s_pre, A_DIM);
    mvwprintw(s_pre, 2, 2, "Empty Slot");
    mvwprintw(s_pre, 3, 2, "No data available.");
    wattroff(s_pre, A_DIM);
  }
  wrefresh(s_pre);
}

void saves_resize() {
  if (!s_win || !s_pre)
    return;

  int total_w = SAVES_WIDTH + 1 + PREVIEW_WIDTH;
  int start_x = (COLS - total_w) / 2;
  int start_y = (LINES - PREVIEW_HEIGHT) / 2;

  mvwin(s_win, start_y, start_x);
  mvwin(s_pre, start_y, start_x + SAVES_WIDTH + 1);

  rebuild_saves_menu();
}

void saves_cleanup() {
  free_menu_ctx(s_win, s_menu, s_items, 3, true);

  if (s_pre) {
    delwin(s_pre);
    s_pre = NULL;
  }
}
