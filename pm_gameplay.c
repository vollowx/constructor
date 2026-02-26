#include "fcp.h"
#include "helpers.h"
#include "log.h"
#include "models.h"
#include "save.h"

typedef struct {
  char symbol;
  // char symbol[2];
  short fg;
  short bg;

  short cp; // Registered color pair at runtime
} CellVisualDef;

// clang-format off
static CellVisualDef CELL_VISUAL_DB[] = {
    [ELEV_NONE]       = { '?', COLOR_BLACK, COLOR_MAGENTA},
    [ELEV_DEEP_WATER] = { '~', COLOR_BLACK, COLOR_BLUE},
    [ELEV_WATER]      = { ' ', COLOR_WHITE, COLOR_BLUE},
    [ELEV_GROUND]     = { ' ', COLOR_BLACK, COLOR_GREEN},
    [ELEV_HILL]       = { '^', COLOR_BLACK, COLOR_GREEN},
    [ELEV_MOUNTAIN]   = { '*', COLOR_BLACK, COLOR_GREEN},
};
// clang-format on

WINDOW *g_win = NULL;
Game current_game = {0};
Save current_save = {0};

bool g_is_first_run = true;
bool g_need_redraw = true;

void gameplay_init() {
  info("[model] model_gameplay initializing");

  if (g_is_first_run) {
    current_save.game = &current_game;
    g_is_first_run = false;

    for (int i = 0; i < _elevation_count; ++i) {
      CELL_VISUAL_DB[i].cp =
          fcp_get(CELL_VISUAL_DB[i].fg, CELL_VISUAL_DB[i].bg);
    }
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
    g_need_redraw = entity_move(p, 0, -1, current_save.game->map);
    break;
  case KEY_DOWN:
    g_need_redraw = entity_move(p, 0, 1, current_save.game->map);
    break;
  case KEY_LEFT:
    g_need_redraw = entity_move(p, -1, 0, current_save.game->map);
    break;
  case KEY_RIGHT:
    g_need_redraw = entity_move(p, 1, 0, current_save.game->map);
    break;
  case 'i':
    // TASK(20260226-155803): Add object related functions
    current_game.map->cells[current_game.player->y + 1][current_game.player->x]
        .object_id = 10000;
    g_need_redraw = true;
    break;
  case 'q':
    next_state = STATE_SAVES;
    break;
  }
}

void gameplay_render() {
  if (!g_win || !current_save.game || !current_save.game->player)
    return;

  if (!g_need_redraw)
    return;

  Entity *p = current_save.game->player;
  Map *map = current_save.game->map;

  static int redraw_count = 0;
  static int fcp_get_calls = 0;
  static int attrset_calls = 0;
  int fcp_get_calls_in_one_render = 0;
  int attrset_calls_in_one_render = 0;

  int sw, sh;
  getmaxyx(g_win, sh, sw);

  int view_w_cells = sw / 2;

  int target_vy = (int)p->y - (sh / 2);
  int target_vx = (int)p->x - (view_w_cells / 2);

  size_t vy = (target_vy < 0) ? 0 : (size_t)target_vy;
  size_t vx = (target_vx < 0) ? 0 : (size_t)target_vx;

  if (vy + sh > map->h)
    vy = (map->h > (size_t)sh) ? map->h - sh : 0;
  if (vx + sw > map->w)
    vx = (map->w > (size_t)view_w_cells) ? map->w - view_w_cells : 0;

  int max_y = (vy + sh > map->h) ? (int)(map->h - vy) : sh;
  int max_x = (vx + view_w_cells > map->w) ? (int)(map->w - vx) : view_w_cells;

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

      const CellVisualDef *cell_def = &CELL_VISUAL_DB[cell->elevation];
      char symbol[2] = {cell_def->symbol, cell_def->symbol};
      short fg = cell_def->fg, bg = cell_def->bg;
      short cp = cell_def->cp;
      int attr = A_NORMAL;

      if (cell->entity) {
        const EntityDef *def = cell->entity->def;
        symbol[0] = def->symbol[0];
        symbol[1] = def->symbol[1];
        if (def->fg != -1)
          fg = def->fg;
        if (def->bg != -1)
          bg = def->bg;
        cp = fcp_get(fg, bg);

        if (def->attr)
          attr = def->attr;

        ++fcp_get_calls;
        ++fcp_get_calls_in_one_render;
      } else if (cell->object_id) {
        const ObjectDef *def = object_get_def(cell->object_id);
        symbol[0] = def->symbol[0];
        symbol[1] = def->symbol[1];
        if (def->fg != -1)
          fg = def->fg;
        if (def->bg != -1)
          bg = def->bg;
        cp = fcp_get(fg, bg);

        if (def->attr)
          attr = def->attr;

        ++fcp_get_calls;
        ++fcp_get_calls_in_one_render;
      }

      attr_t new_attrs = COLOR_PAIR(cp) | attr;

      if (new_attrs != current_attrs) {
        wattrset(g_win, new_attrs);
        current_attrs = new_attrs;
        ++attrset_calls;
        ++attrset_calls_in_one_render;
      }

      waddch(g_win, symbol[0]);
      waddch(g_win, symbol[1]);
    }
  }

  wattrset(g_win, A_NORMAL);

  mvwprintw(g_win, 0, 0, "cell: { elevation: %d }",
            map->cells[p->y][p->x].elevation);
  mvwprintw(g_win, 1, 0, "player: { x: %zu, y: %zu }", p->x, p->y);
  mvwprintw(g_win, 2, 0, "entities: %zu", current_save.game->entities.count);
  mvwprintw(g_win, 3, 0, "redraws: %d", ++redraw_count);
  mvwprintw(g_win, 4, 0, "fcp_get calls: %d / %d", fcp_get_calls_in_one_render,
            fcp_get_calls);
  mvwprintw(g_win, 5, 0, "attrset calls: %d / %d", attrset_calls_in_one_render,
            attrset_calls);

  wnoutrefresh(g_win);

  g_need_redraw = false;
}

void gameplay_resize() {
  if (g_win) {
    wresize(g_win, LINES, COLS);
    mvwin(g_win, 0, 0);
  }
}

void gameplay_cleanup() {
  werase(g_win);
  wnoutrefresh(g_win);
  if (g_win) {
    delwin(g_win);
    g_win = NULL;
  }
  free_game(current_save.game);
}
