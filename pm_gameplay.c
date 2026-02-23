#include "fcp.h"
#include "helpers.h"
#include "log.h"
#include "models.h"
#include "save.h"

WINDOW *g_win = NULL;
Game current_game = {0};
Save current_save = {0};

bool gameplay_is_first_run = true;
bool gameplay_need_redraw = true;

void gameplay_init() {
  info("[model] model_gameplay initializing");

  if (gameplay_is_first_run) {
    current_save.game = &current_game;
    gameplay_is_first_run = false;
  }

  if (save_load(&current_save, current_slot) != SAVE_OK) {
    next_state = STATE_MAIN_MENU;
    return;
  }

  g_win = newwin(LINES, COLS, 0, 0);
  keypad(g_win, TRUE);
}

void gameplay_input(int ch) {
  Entity *p = current_save.game->player;
  if (!p)
    return;

  switch (ch) {
  case KEY_UP:
    gameplay_need_redraw = entity_move(p, 0, -1, current_save.game->map);
    break;
  case KEY_DOWN:
    gameplay_need_redraw = entity_move(p, 0, 1, current_save.game->map);
    break;
  case KEY_LEFT:
    gameplay_need_redraw = entity_move(p, -1, 0, current_save.game->map);
    break;
  case KEY_RIGHT:
    gameplay_need_redraw = entity_move(p, 1, 0, current_save.game->map);
    break;
  case 'q':
    next_state = STATE_MAIN_MENU;
    break;
  }
}

void gameplay_render() {
  if (!g_win || !current_save.game || !current_save.game->player)
    return;

  if (!gameplay_need_redraw)
    return;

  int sw, sh;
  getmaxyx(g_win, sh, sw);

  Entity *p = current_save.game->player;
  Map *map = current_save.game->map;

  int target_vx = (int)p->x - (sw / 2);
  int target_vy = (int)p->y - (sh / 2);

  size_t vx = (target_vx < 0) ? 0 : (size_t)target_vx;
  size_t vy = (target_vy < 0) ? 0 : (size_t)target_vy;

  if (vx + sw > map->w)
    vx = (map->w > (size_t)sw) ? map->w - sw : 0;
  if (vy + sh > map->h)
    vy = (map->h > (size_t)sh) ? map->h - sh : 0;

  int max_y = (vy + sh > map->h) ? (int)(map->h - vy) : sh;
  int max_x = (vx + sw > map->w) ? (int)(map->w - vx) : sw;

  if (max_y < sh || max_x < sw) {
    werase(g_win);
  }

  attr_t current_attrs = A_NORMAL;
  wattrset(g_win, current_attrs);

  // TODO: Move into game_tick or on player movements
  // game_gen_area(current_save.game, vy, vx, vy + sh, vx + sw);

  for (int y = 0; y < max_y; y++) {
    wmove(g_win, y, 0);
    size_t wy = vy + y;

    for (int x = 0; x < max_x; x++) {
      size_t wx = vx + x;
      MapCell *cell = &map->cells[wy][wx];

      char symbol = '?';
      short fg = COLOR_BLACK, bg = COLOR_MAGENTA;
      int attr = A_NORMAL;

      switch ((int)cell->elevation) {
      case ELEV_GROUND:
        symbol = ' ';
        fg = COLOR_BLACK;
        bg = COLOR_GREEN;
        break;
      case ELEV_WATER:
        symbol = ' ';
        bg = COLOR_BLUE;
        break;
      case ELEV_DEEP_WATER:
        symbol = '~';
        fg = COLOR_BLACK;
        bg = COLOR_BLUE;
        break;
      }

      if (cell->entity) {
        if (cell->entity->type == ENT_PLAYER) {
          symbol = '@';
          fg = COLOR_BLUE;
          attr = A_BOLD;
        } else {
          // TODO: Should use ENT_DB, does not exist now
          const ItemDef *def = &ITEM_DB[cell->entity->id];
          symbol = def->symbol;
          if (def->fg != 0)
            fg = def->fg;
          if (def->bg != 0)
            bg = def->bg;
        }
      } else if (cell->object) {
        const ObjectDef *def = &OBJ_DB[cell->object->id];
        symbol = def->symbol;
        if (def->fg != 0)
          fg = def->fg;
        if (def->bg != 0)
          bg = def->bg;
      }

      short cp = fcp_get(fg, bg);
      attr_t new_attrs = COLOR_PAIR(cp) | attr;

      if (new_attrs != current_attrs) {
        wattrset(g_win, new_attrs);
        current_attrs = new_attrs;
      }

      waddch(g_win, symbol);
    }
  }

  wattrset(g_win, A_NORMAL);

  static int redraw_count = 0;

  mvwprintw(g_win, 0, 0, "cell: { elevation: %d }", map->cells[p->y][p->x].elevation);
  mvwprintw(g_win, 1, 0, "player: { x: %zu, y: %zu }", p->x, p->y);
  mvwprintw(g_win, 2, 0, "entities: %zu",
            current_save.game->entities.count);
  mvwprintw(g_win, 3, 0, "redraw count: %d", ++redraw_count);

  wrefresh(g_win);

  gameplay_need_redraw = false;
}

void gameplay_resize() {
  if (g_win) {
    wresize(g_win, LINES, COLS);
    mvwin(g_win, 0, 0);
  }
}

void gameplay_cleanup() {
  if (g_win) {
    delwin(g_win);
    g_win = NULL;
  }
  free_game(current_save.game);
}
