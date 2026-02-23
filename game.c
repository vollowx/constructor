#include <time.h>

#include <ncurses.h>
#include <simplexnoise1234.h>

#include "game.h"
#include "helpers.h"

// clang-format off
const ItemDef ITEM_DB[] = {
    {.id = 0,     .type = ITEM_RESOURCE,   .name = "Copper Ore", .symbol = 'c', .fg = COLOR_YELLOW,  .max_stack = 64},
    {.id = 1,     .type = ITEM_RESOURCE,   .name = "Iron Ore",   .symbol = 'o', .fg = COLOR_WHITE,   .max_stack = 64},
    {.id = 2,     .type = ITEM_RESOURCE,   .name = "Gold Ore",   .symbol = 'o', .fg = COLOR_YELLOW,  .max_stack = 64},
    {.id = 10000, .type = ITEM_PLACEABLE,  .name = "Brick",      .symbol = 'N', .fg = COLOR_WHITE,   .max_stack = 64},
    {.id = 20000, .type = ITEM_CONSUMABLE, .name = "Apple",      .symbol = 'O', .fg = COLOR_RED,     .max_stack = 24},
    {.id = 20001, .type = ITEM_CONSUMABLE, .name = "Orange",     .symbol = 'O', .fg = COLOR_YELLOW,  .max_stack = 24},
    {.id = 20002, .type = ITEM_CONSUMABLE, .name = "Berry",      .symbol = 'o', .fg = COLOR_YELLOW,  .max_stack = 24},
    {.id = 30000, .type = ITEM_EQUIPMENT,  .name = "Copper Axe", .symbol = 'F', .fg = COLOR_YELLOW},
    {.id = 30001, .type = ITEM_EQUIPMENT,  .name = "Iron Axe",   .symbol = 'F', .fg = COLOR_WHITE },
    {.id = 30002, .type = ITEM_EQUIPMENT,  .name = "Gold Axe",   .symbol = 'F', .fg = COLOR_YELLOW},
};

const ObjectDef OBJ_DB[] = {
    {.id = 0, .name = "Stone Boulder", .max_health = 5,  .is_passable = false, .symbol = 'O', .fg = COLOR_BLACK, .bg = COLOR_WHITE },
    {.id = 1, .name = "Wood Bridge",   .max_health = 1,  .is_passable = true , .symbol = '#', .fg = COLOR_BLACK, .bg = COLOR_YELLOW},
    {.id = 2, .name = "Furnace",       .max_health = 10, .is_passable = false, .symbol = '&', .fg = COLOR_BLACK, .bg = COLOR_WHITE },
};
// clang-format on

const ItemDef *item_get_def(int id) {
  for (size_t i = 0; i < sizeof(ITEM_DB) / sizeof(ItemDef); ++i) {
    if (ITEM_DB[i].id == id)
      return &ITEM_DB[i];
  }
  return NULL;
}

char item_get_symbol(int id) {
  const ItemDef *def = item_get_def(id);
  return def ? def->symbol : '?';
}

bool entity_move(Entity *e, int dx, int dy, Map *map) {
  if (!e || !map)
    return false;

  if (dx < 0 && e->x < (size_t)abs(dx))
    return false;
  if (dy < 0 && e->y < (size_t)abs(dy))
    return false;

  size_t new_x = e->x + dx;
  size_t new_y = e->y + dy;

  if (new_x >= map->w || new_y >= map->h)
    return false;

  MapCell *target = &map->cells[new_y][new_x];

  if (target->entity != NULL)
    return false;
  if (target->elevation == ELEV_DEEP_WATER || target->elevation == ELEV_WATER)
    return false;

  map->cells[e->y][e->x].entity = NULL;

  e->x = new_x;
  e->y = new_y;
  e->z = target->elevation;

  target->entity = e;

  return true;
}

Map *new_map(size_t height, size_t width) {
  // 1. Allocate the Map container
  Map *map = malloc(sizeof(Map));
  if (!map)
    return NULL;

  map->w = width;
  map->h = height;

  // 2. Allocate the array of row pointers
  map->cells = malloc(sizeof(MapCell *) * height);
  if (!map->cells) {
    free(map);
    return NULL;
  }

  // 3. Allocate each row
  for (size_t y = 0; y < height; ++y) {
    // Use calloc to automatically zero out all members (Elevation 0, NULL
    // pointers)
    map->cells[y] = calloc(width, sizeof(MapCell));

    if (!map->cells[y]) {
      // Cleanup previous rows on failure
      for (size_t i = 0; i < y; ++i)
        free(map->cells[i]);
      free(map->cells);
      free(map);
      return NULL;
    }
  }

  return map;
}

void free_map(Map *map) {
  if (!map)
    return;

  for (size_t y = 0; y < map->h; ++y) {
    free(map->cells[y]);
  }
  free(map->cells);
  free(map);
}

// TASK(20260223-173936): Chunk-ize game both in struct and file
void game_init(Game *game) {
  game->map = new_map(2048, 2048);

  game->entities.items = NULL;
  game->entities.count = 0;
  game->entities.capacity = 0;

  Entity *player = malloc(sizeof(Entity));
  memset(player, 0, sizeof(Entity));

  player->id = 0;
  player->type = ENT_PLAYER;
  player->health = 100;
  player->health_max = 100;
  player->z = ELEV_GROUND;
  strncpy(player->name, "Default Name", sizeof(player->name) - 1);

  player->inventory.count = 0;
  player->inventory.capacity = 8;
  player->inventory.items =
      malloc(sizeof(ItemStack) * player->inventory.capacity);

  da_append(&game->entities, player);
  game->player = player;

  srand((unsigned int)time(NULL));
  game->seed = ((uint32_t)rand() << 16) | (uint32_t)rand();

  size_t py = 1024, px = 1024, y_offset = 0;
  game_gen_area(game, py - 256, px - 256, py + 256, px + 256);

  // TODO: Fails in a rather small possibility
  while (game->map->cells[py + y_offset][px].elevation != ELEV_GROUND &&
         y_offset < 256) {
    ++y_offset;
  }
  py += y_offset;

  player->y = py;
  player->x = px;
  game->map->cells[player->y][player->x].entity = player;
}

void free_game(Game *game) {
  if (!game)
    return;

  if (game->map) {
    free_map(game->map);
    game->map = NULL;
  }

  da_foreach(Entity *, it, &game->entities) {
    Entity *ent = *it;
    if (ent) {
      if (ent->inventory.items) {
        free(ent->inventory.items);
      }
      free(ent);
    }
  }

  if (game->entities.items) {
    free(game->entities.items);
    game->entities.items = NULL;
  }

  game->entities.count = 0;
  game->entities.capacity = 0;
  game->player = NULL;
}

void game_gen_area(Game *game, size_t start_y, size_t start_x, size_t end_y,
                   size_t end_x) {
  if (!game || !game->map)
    return;
  Map *map = game->map;

  float seed_ox = (float)(game->seed % 100000);
  float seed_oy = (float)((game->seed / 100) % 100000);

  for (size_t y = start_y; y < end_y && y < map->h; y++) {
    for (size_t x = start_x; x < end_x && x < map->w; x++) {
      MapCell *cell = &map->cells[y][x];

      if (cell->elevation == ELEV_NONE) {
        float scale = 0.07f;
        float raw_noise =
            snoise2((float)x * scale + seed_ox, (float)y * scale + seed_oy);
        float val = (raw_noise + 1.0f) * 0.5f;

        if (val > 0.9f) {
          cell->elevation = ELEV_MOUNTAIN;
        } else if (val > 0.85f) {
          cell->elevation = ELEV_HILL;
        } else if (val > 0.4f) {
          cell->elevation = ELEV_GROUND;
        } else if (val > 0.3f) {
          cell->elevation = ELEV_WATER;
        } else {
          cell->elevation = ELEV_DEEP_WATER;
        }

        if (cell->elevation == ELEV_GROUND || cell->elevation == ELEV_HILL) {
          uint32_t res_h = game->seed ^ ((uint32_t)x * 123456789U) ^
                           ((uint32_t)y * 987654321U);

          uint32_t spawn_threshold = (cell->elevation == ELEV_HILL) ? 7U : 4U;

          if ((res_h % 100) < spawn_threshold) {
            Entity *res = calloc(1, sizeof(Entity));
            if (res) {
              res->type = ENT_MOB;
              res->x = x;
              res->y = y;

              int pick = (res_h >> 8) % 3;
              res->id = ITEM_DB[pick].id;
              strncpy(res->name, ITEM_DB[pick].name, sizeof(res->name) - 1);

              cell->entity = res;
              da_append(&game->entities, res);
            }
          }
        }
      }
    }
  }
}
