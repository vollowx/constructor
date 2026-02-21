#include <menu.h>

#include "log.h"
#include "models.h"
#include <string.h>

#include "save.h" // For SaveHeader
#include "states.h"

#define SAVES_HEIGHT 10
#define SAVES_WIDTH 30
#define PREVIEW_WIDTH 40

static ITEM **s_items;
static MENU *s_menu;
static WINDOW *s_win;
static WINDOW *s_pre;
static SavePreview previews[3];

void rebuild_saves_menu() {
  if (s_menu) {
    unpost_menu(s_menu);
    free_menu(s_menu);
    for (int i = 0; i < 3; i++)
      free_item(s_items[i]);
    free(s_items);
  }

  s_items = (ITEM **)calloc(4, sizeof(ITEM *));
  for (int i = 0; i < 3; i++) {
    previews[i] = get_slot_preview(i);
    char label[64];
    if (previews[i].exists) {
      snprintf(label, sizeof(label), "Slot %d: %s", i + 1,
               previews[i].header.player_name);
    } else {
      snprintf(label, sizeof(label), "Slot %d: <Empty>", i + 1);
    }
    s_items[i] = new_item(strdup(label), "");
  }
  s_items[3] = (ITEM *)NULL;

  s_menu = new_menu(s_items);
  set_menu_win(s_menu, s_win);
  set_menu_sub(s_menu, derwin(s_win, 3, SAVES_WIDTH - 2, 2, 1));
  set_menu_mark(s_menu, " > ");
  post_menu(s_menu);
}

void saves_init() {
  info("primary model switched to saves");

  int start_y = (LINES - SAVES_HEIGHT) / 2;
  int start_x = (COLS - (SAVES_WIDTH + PREVIEW_WIDTH)) / 2;

  s_win = newwin(SAVES_HEIGHT, SAVES_WIDTH, start_y, start_x);
  s_pre =
      newwin(SAVES_HEIGHT, PREVIEW_WIDTH, start_y, start_x + SAVES_WIDTH + 1);

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
    info("Rename requested for slot %d", idx);
    if (previews[idx].exists) {
      // current_slot = ...
      // next_state = STATE_SAVE_RENAME
      // For now, simple log
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
  // Render Left Menu Window
  box(s_win, 0, 0);
  mvwprintw(s_win, 0, 2, " Select Save ");
  mvwprintw(s_win, SAVES_HEIGHT - 2, 2, "[Ent]Play [X]Del [R]Ren");
  wrefresh(s_win);

  // Render Right Preview Window
  box(s_pre, 0, 0);
  mvwprintw(s_pre, 0, 2, " Preview ");

  int idx = item_index(current_item(s_menu));
  if (previews[idx].exists) {

    mvwprintw(s_pre, 2, 2, "Name: %s", previews[idx].header.player_name);
    mvwprintw(s_pre, 3, 2, "Version: %d", previews[idx].header.version);

    // Add map or pet info here from the header
  } else {

    mvwprintw(s_pre, 2, 2, "No data found.");
  }
  wrefresh(s_pre);
}

void saves_cleanup() {
  unpost_menu(s_menu);
  free_menu(s_menu);
  for (int i = 0; i < 3; i++)
    free_item(s_items[i]);
  free(s_items);
  delwin(s_win);
  delwin(s_pre);
}

void saves_resize() {
  // Similar to options_resize, re-calculate start_y and start_x
}

DEFINE_MODEL(saves);
