#include <string.h>

#include <menu.h>

#include "fcp.h"
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
  set_menu_fore(s_menu, COLOR_PAIR(fcp_get(COLOR_BLUE, COLOR_BLACK)) | A_BOLD |
                            A_REVERSE);
  set_current_item(s_menu, s_items[current_slot]);

  post_menu(s_menu);
}

void saves_init() {
  info("[model] model_saves initializing");

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

void saves_deinit() {
  werase(s_win);
  werase(s_pre);
  wnoutrefresh(s_win);
  wnoutrefresh(s_pre);

  free_menu_ctx(s_win, s_menu, s_items, 3, true);

  if (s_pre) {
    delwin(s_pre);
    s_pre = NULL;
  }
}

void saves_input(int ch) {
  int slot = item_index(current_item(s_menu));

  ITEM *cur = current_item(s_menu);
  if (cur) {
    current_slot = item_index(cur);
  }

  switch (ch) {
  case KEY_DOWN:
  case 'j':
    menu_driver(s_menu, REQ_DOWN_ITEM);
    break;
  case KEY_UP:
  case 'k':
    menu_driver(s_menu, REQ_UP_ITEM);
    break;
  case 'q':
    next_state = STATE_MAIN_MENU;
    break;

  case 10:
  case 'o':
    if (previews[slot].exists) {
      next_state = STATE_GAMEPLAY;
    } else {
      Game game = {0};
      game_init(&game);
      Save save = {0};
      save.game = &game;
      save_init(&save);
      strcpy(save.header.player_name, game.player->name);
      save_save(&save, slot);
      free_game(&game);

      rebuild_saves_menu();
    }

    break;

  case 'x':
    // next_state = STATE_SAVE_DELETE
    break;
  case 'r':
    if (!previews[slot].exists)
      break;

    echo();
    curs_set(1);

    char new_name[32];

    mvwprintw(s_pre, 2, 22, "%32s", "");
    wmove(s_pre, 2, 22);
    wgetnstr(s_pre, new_name, 31);

    noecho();
    curs_set(0);

    if (strlen(new_name) > 0) {
      Game game = {0};
      Save save = {0};
      save.game = &game;
      if (save_load(&save, slot) == SAVE_OK) {
        strcpy(save.header.player_name, new_name);
        strcpy(game.player->name, new_name);
        if (save_save(&save, slot) == SAVE_OK) {
          info("[save] Renamed slot %d to %s", slot, new_name);
        } else {
          error("[save] Failed to rename slot saving save");
        }
      } else {
        error("[save] Failed to rename slot loading save");
      }
      free_game(save.game);
    }

    rebuild_saves_menu();
    break;
  }
}

void saves_frame(double dt) {
  UNUSED(dt);

  if (!s_win || !s_pre)
    return;

  draw_win_frame(s_win, "Select Save", COLOR_BLUE);
  wnoutrefresh(s_win);

  werase(s_pre);
  draw_win_frame(s_pre, "Preview", COLOR_CYAN);

  int idx = item_index(current_item(s_menu));
  if (previews[idx].exists) {
    wattron(s_pre, COLOR_PAIR(1));
    mvwprintw(s_pre, 2, 2, "%16s", "player");
    wattroff(s_pre, COLOR_PAIR(1));
    mvwprintw(s_pre, 2, 22, "%s", previews[idx].header.player_name);

    wattron(s_pre, A_DIM);
    mvwhline(s_pre, 3, 22, ACS_HLINE, 31);
    wattroff(s_pre, A_DIM);

    mvwprintw(s_pre, 4, 2, "%16s", "version");
    mvwprintw(s_pre, 4, 22, "%d", previews[idx].header.version);
  } else {
    wattron(s_pre, A_DIM);
    mvwprintw(s_pre, 2, 4, "Empty Slot");
    mvwprintw(s_pre, 3, 4, "No data available.");
    wattroff(s_pre, A_DIM);
  }
  wnoutrefresh(s_pre);
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
