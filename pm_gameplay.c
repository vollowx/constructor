#include "fcp.h"
#include "helpers.h"
#include "log.h"
#include "models.h"
#include "save.h"

WINDOW *g_win = NULL;
Game current_game = {0};
Save current_save = {0};

bool gameplay_is_first_run = true;

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
    entity_move(p, 0, -1, current_save.game->map);
    break;
  case KEY_DOWN:
    entity_move(p, 0, 1, current_save.game->map);
    break;
  case KEY_LEFT:
    entity_move(p, -1, 0, current_save.game->map);
    break;
  case KEY_RIGHT:
    entity_move(p, 1, 0, current_save.game->map);
    break;
  case 'q':
    next_state = STATE_MAIN_MENU;
    break;
  }
}

void render_entity(WINDOW *win, Entity *ent, int screen_y, int screen_x) {
  if (!ent)
    return;

  int cp = fcp_get(COLOR_BLACK, COLOR_MAGENTA);
  int attr = A_NORMAL;
  char sym = '?';

  if (ent->type == ENT_PLAYER) {
    sym = '@';
    cp = 1;
    fcp_get(COLOR_BLUE, -1);
    attr = A_BOLD;
  } else {
    // TODO: Should use ENT_DB, does not exist now
    const ItemDef *def = &ITEM_DB[ent->id];
    sym = def->symbol;
    cp = fcp_get(def->fg, def->bg);
  }

  wattron(win, COLOR_PAIR(cp) | attr);
  mvwaddch(win, screen_y, screen_x, sym);
  wattroff(win, COLOR_PAIR(cp) | attr);
}

void gameplay_render() {
  if (!g_win || !current_save.game || !current_save.game->player)
    return;

  werase(g_win);

  int sw, sh;
  getmaxyx(g_win, sh, sw);

  Entity *p = current_save.game->player;
  Map *map = current_save.game->map;

  int target_vx = (int)p->x - (sw / 2);
  int target_vy = (int)p->y - (sh / 2);

  // Clamp viewport to Map boundaries
  if (target_vx < 0)
    target_vx = 0;
  if (target_vy < 0)
    target_vy = 0;

  if ((size_t)(target_vx + sw) > map->w)
    target_vx = (int)map->w - sw;
  if ((size_t)(target_vy + sh) > map->h)
    target_vy = (int)map->h - sh;

  // If map is smaller than screen
  size_t vx = (target_vx < 0) ? 0 : (size_t)target_vx;
  size_t vy = (target_vy < 0) ? 0 : (size_t)target_vy;

  game_generate_area(current_save.game, vx, vy, vx + sw, vy + sh,
                     current_save.header.timestamp);

  for (int y = 0; y < sh; y++) {
    for (int x = 0; x < sw; x++) {
      size_t wx = vx + x;
      size_t wy = vy + y;
      if (wx >= map->w || wy >= map->h)
        continue;

      MapCell *cell = &map->cells[wy][wx];

      short fg = COLOR_WHITE, bg = COLOR_BLACK;
      char symbol = ' ';

      switch ((int)cell->elevation) {
      case ELEV_GROUND:
        symbol = '%';
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
        render_entity(g_win, cell->entity, y, x);
        continue;
      } else if (cell->object) {
        const ObjectDef *def = &OBJ_DB[cell->object->id];
        symbol = def->symbol;
        if (def->fg != 0)
          fg = def->fg;
        if (def->bg != 0)
          bg = def->bg;
      }

      short cp = fcp_get(fg, bg);
      wattron(g_win, COLOR_PAIR(cp));
      mvwaddch(g_win, y, x, symbol);
      wattroff(g_win, COLOR_PAIR(cp));
    }
  }

  // Tiny HUD to show movement
  mvwprintw(g_win, 0, 0, "[ %zu, %zu ]", p->x, p->y);
  mvwprintw(g_win, 1, 0, "Entities in memory: %zu",
            current_save.game->entities.count);
  wrefresh(g_win);
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
