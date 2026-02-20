#include <stdlib.h>
#include <string.h>

#include <menu.h>

#include "models.h"
#include "options.h"
#include "states.h"

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
#define OPTIONS_WIDTH 48

extern GameState next_state;

static ITEM **opt_items;
static MENU *opt_menu;
static WINDOW *opt_win;

ITEM *new_caption(const char *title) {
  ITEM *item = new_item(title, "");
  item_opts_off(item, O_SELECTABLE);
  return item;
}

void format_menu_item(char *dest, size_t size, const char *label,
                      const char *value) {
  int label_width = 16;
  int total_width = OPTIONS_WIDTH - 4;
  snprintf(dest, size, "%-*s %*s", label_width, label,
           (total_width - label_width), value);
}

void rebuild_options_menu() {
  int current_idx = 0;

  if (opt_menu) {
    ITEM *cur = current_item(opt_menu);
    if (cur) {
      current_idx = item_index(cur);
    }
    unpost_menu(opt_menu);
    free_menu(opt_menu);
  }

  opt_items = (ITEM **)calloc(OPTIONS_HEIGHT + 1, sizeof(ITEM *));
  int i = 0;

  opt_items[i++] = new_caption("  Game");
  opt_items[i++] = new_item("Foo", "");
  opt_items[i++] = new_item("Bar", "");
  opt_items[i++] = new_item("Baz", "");

  opt_items[i++] = new_caption("  Display");
  opt_items[i++] = new_item("Zoom", "");

  opt_items[i++] = new_caption("  Other");

  static char log_level_line[64], log_show_line[64], log_save_line[64];
  const char *lvls[] = {"<Information>", "<Warning>", "<Error>"};

  format_menu_item(log_level_line, sizeof(log_level_line), "Show log level",
                   lvls[current_options.log_level]);
  format_menu_item(log_show_line, sizeof(log_show_line), "Show log window",
                   current_options.show_log ? "[x]" : "[ ]");
  format_menu_item(log_save_line, sizeof(log_save_line), "Save log locally",
                   current_options.save_log ? "[x]" : "[ ]");

  opt_items[i++] = new_item(log_level_line, "");
  opt_items[i++] = new_item(log_show_line, "");
  opt_items[i++] = new_item(log_save_line, "");
  opt_items[i++] = new_item("Clear logs", "");
  opt_items[i++] = new_item("Clear local logs", "");

  opt_items[i++] = new_caption(" ");
  opt_items[i++] = new_item("Save", "");
  opt_items[i++] = new_item("Cancel", "");
  opt_items[i++] = NULL;

  opt_menu = new_menu(opt_items);
  set_menu_win(opt_menu, opt_win);
  set_menu_sub(opt_menu, derwin(opt_win, OPTIONS_HEIGHT, OPTIONS_WIDTH, 2, 1));
  set_menu_mark(opt_menu, " > ");
  set_current_item(opt_menu, opt_items[current_idx]);

  post_menu(opt_menu);
}

void options_init() {
  opt_win =
      newwin(OPTIONS_HEIGHT + 4, OPTIONS_WIDTH + 4,
             (LINES + 2 - OPTIONS_HEIGHT) / 2, (COLS + 2 - OPTIONS_WIDTH) / 2);
  keypad(opt_win, TRUE);
  rebuild_options_menu();
}

void options_input(int ch) {
  switch (ch) {
  case KEY_DOWN:
    menu_driver(opt_menu, REQ_DOWN_ITEM);
    break;
  case KEY_UP:
    menu_driver(opt_menu, REQ_UP_ITEM);
    break;
  case 10: {
    ITEM *cur = current_item(opt_menu);
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
  box(opt_win, 0, 0);
  mvwprintw(opt_win, 0, 3, " Options ");
  wrefresh(opt_win);
}

void options_cleanup() {
  if (opt_menu) {
    unpost_menu(opt_menu);
    free_menu(opt_menu);
    opt_menu = NULL;
  }
  if (opt_items) {
    for (int i = 0; opt_items[i]; i++)
      free_item(opt_items[i]);
    free(opt_items);
    opt_items = NULL;
  }
  if (opt_win) {
    delwin(opt_win);
    opt_win = NULL;
  }
}

void options_resize() {
  if (opt_win)
    mvwin(opt_win, (LINES + 2 - OPTIONS_HEIGHT) / 2,
          (COLS + 2 - OPTIONS_WIDTH) / 2);
}

DEFINE_MODEL(options);
