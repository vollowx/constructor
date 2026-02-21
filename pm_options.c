#include <stdlib.h>
#include <string.h>

#include <menu.h>

#include "helpers.h"
#include "log.h"
#include "models.h"
#include "options.h"

// Looks like:
//
//  ┌── Options ─────────────────────────(52)──────────┐
//  │                                    (48)          │
//  │ >   Game                                         │
//  │   Foo                                            │
//  │   Bar                                            │
//  │   Baz                                            │
//  │     Display                                      │
//  │   Zoom                                           │
//  │     Other                                        │
//  │   Show log level                  <Information>  │
//  │   Show log window                           [x]  │
//  │                                                  │
//  │   Save                                           │
//  │   Cancel                                         │
//  │                                                  │
//  │                                                  │
//  │                                         (32)   (36)
//  │                                                  │
//  │                                                  │
//  │                                                  │
//  │                                                  │
//  │                                                  │
//  │                                                  │
//  │                                                  │
//  │                                                  │
//  │                                                  │
//  │                                                  │
//  │                                                  │
//  │                                                  │
//  │                                                  │
//  │                                                  │
//  │                                                  │
//  │                                                  │
//  │                                                  │
//  │                                                  │
//  └──────────────────────────────────────────────────┘

#define OPTIONS_HEIGHT 32
#define OPTIONS_WIDTH 56

static ITEM **o_items = NULL;
static MENU *o_menu = NULL;
static WINDOW *o_win = NULL;

ITEM *new_caption(const char *title) {
  ITEM *item = new_item(title, "");
  item_opts_off(item, O_SELECTABLE);
  return item;
}

void format_menu_item(char *dest, size_t size, const char *label,
                      const char *value) {
  int label_width = 16;
  int total_width = OPTIONS_WIDTH - 8;
  snprintf(dest, size, "%-*s %*s", label_width, label,
           (total_width - label_width), value);
}

void rebuild_options_menu() {
  int current_idx = 0;

  if (o_menu) {
    ITEM *cur = current_item(o_menu);
    if (cur) {
      current_idx = item_index(cur);
    }
    unpost_menu(o_menu);
    free_menu(o_menu);
    o_menu = NULL;
  }
  if (o_items) {
    for (int i = 0; i < OPTIONS_HEIGHT - 4 + 1; i++) {
      free_item(o_items[i]);
    }
    free(o_items);
    o_items = NULL;
  }

  o_items = (ITEM **)calloc(OPTIONS_HEIGHT - 4 + 1, sizeof(ITEM *));
  int i = 0;

  o_items[i++] = new_caption("  Game");
  o_items[i++] = new_item("Foo", "");
  o_items[i++] = new_item("Bar", "");
  o_items[i++] = new_item("Baz", "");

  o_items[i++] = new_caption("  Display");
  o_items[i++] = new_item("Zoom", "");

  o_items[i++] = new_caption("  Other");

  static char log_level_line[64], log_show_line[64], log_save_line[64];
  const char *lvls[] = {"<Information>", "<Warning>", "<Error>"};

  format_menu_item(log_level_line, sizeof(log_level_line), "Show log level",
                   lvls[current_options.log_level]);
  format_menu_item(log_show_line, sizeof(log_show_line), "Show log window",
                   current_options.show_log ? "[x]" : "[ ]");
  format_menu_item(log_save_line, sizeof(log_save_line), "Save log locally",
                   current_options.save_log ? "[x]" : "[ ]");

  o_items[i++] = new_item(log_level_line, "");
  o_items[i++] = new_item(log_show_line, "");
  o_items[i++] = new_item(log_save_line, "");
  o_items[i++] = new_item("Clear logs", "");
  o_items[i++] = new_item("Clear local logs", "");

  o_items[i++] = new_caption(" ");
  o_items[i++] = new_item("Save", "");
  o_items[i++] = new_item("Cancel", "");
  o_items[i++] = NULL;

  o_menu = new_menu(o_items);
  set_menu_win(o_menu, o_win);
  set_menu_sub(o_menu, derwin(o_win, OPTIONS_HEIGHT - 4, OPTIONS_WIDTH - 4, 2, 1));
  set_menu_mark(o_menu, " > ");
  set_current_item(o_menu, o_items[current_idx]);

  post_menu(o_menu);
}

void options_init() {
  info("[model] model_options initializing");

  o_win = newwin(OPTIONS_HEIGHT, OPTIONS_WIDTH, (LINES - OPTIONS_HEIGHT) / 2,
                 (COLS - OPTIONS_WIDTH) / 2);
  keypad(o_win, TRUE);
  rebuild_options_menu();
}

void options_input(int ch) {
  switch (ch) {
  case KEY_DOWN:
    menu_driver(o_menu, REQ_DOWN_ITEM);
    break;
  case KEY_UP:
    menu_driver(o_menu, REQ_UP_ITEM);
    break;
  case 'q':
    options_load();
    next_state = STATE_MAIN_MENU;
    break;
  case 10: {
    ITEM *cur = current_item(o_menu);
    const char *name = item_name(cur);

    if (strstr(name, "Show log level")) {
      current_options.log_level = (current_options.log_level + 1) % 3;
      rebuild_options_menu();
    } else if (strstr(name, "Show log window")) {
      erase();
      current_options.show_log = !current_options.show_log;
      rebuild_options_menu();
      refresh();
    } else if (strcmp(name, "Save") == 0) {
      options_save();
      next_state = STATE_MAIN_MENU;
    } else if (strcmp(name, "Cancel") == 0) {
      options_load();
      next_state = STATE_MAIN_MENU;
    }
    break;
  }
  }
}

void options_render() {
  box(o_win, 0, 0);
  mvwprintw(o_win, 0, 3, " Options ");
  wrefresh(o_win);
}

void options_resize() {
  if (o_win)
    mvwin(o_win, (LINES - OPTIONS_HEIGHT) / 2, (COLS - OPTIONS_WIDTH) / 2);
}

void options_cleanup() {
  free_menu_ctx(o_win, o_menu, o_items, OPTIONS_HEIGHT - 4, false);
}
