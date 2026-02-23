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

// TODO: Chunk game both in struct and file
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

float noise2d(int x, int y, uint32_t seed) {
  int n = x + y * 57 + seed;
  n = (n << 13) ^ n;
  // Standard deterministic hash to get a float between 0 and 1
  float res =
      (1.0 - ((n * (n * n * 15731 + 789221) + 1376312589) & 0x7fffffff) /
                 1073741824.0);
  return (res + 1.0f) / 2.0f;
}

// Linearly interpolate between a and b
float lerp(float a, float b, float t) { return a + t * (b - a); }

// Smoothly interpolate for a more organic feel
float smooth_noise(float x, float y, uint32_t seed) {
  int i = (int)x;
  int j = (int)y;
  float fx = x - i;
  float fy = y - j;

  // Corner values
  float a = noise2d(i, j, seed);

  float b = noise2d(i + 1, j, seed);
  float c = noise2d(i, j + 1, seed);

  float d = noise2d(i + 1, j + 1, seed);

  // Bi-linear interpolation
  float ux = fx * fx * (3 - 2 * fx);
  float uy = fy * fy * (3 - 2 * fy);

  return lerp(a, b, ux) + (c - a) * uy * (1 - ux) + (d - b) * ux * uy;
}

void game_gen_area(Game *game, size_t start_y, size_t start_x, size_t end_y,
                   size_t end_x) {
  if (!game || !game->map)
    return;
  Map *map = game->map;

  for (size_t y = start_y; y < end_y && y < map->h; y++) {
    for (size_t x = start_x; x < end_x && x < map->w; x++) {
      MapCell *cell = &map->cells[y][x];

      // Only generate if untouched

      if (cell->elevation == ELEV_NONE) {
        // Scale controls island size: higher = smaller, more frequent islands
        float scale = 0.15f;
        float val =
            smooth_noise((float)x * scale, (float)y * scale, game->seed);

        // Thresholds for Forager-style layers
        if (val > 0.55f) {

          cell->elevation = ELEV_GROUND;
        } else if (val > 0.45f) {
          cell->elevation = ELEV_WATER;
        } else {
          cell->elevation = ELEV_DEEP_WATER;
        }

        // RESOURCE PASS: Only on new ground
        if (cell->elevation == ELEV_GROUND) {
          // Use a different seed offset for resources so they don't follow the
          // coastline perfectly
          uint32_t res_h = game->seed ^ ((uint32_t)x * 123456789U) ^
                           ((uint32_t)y * 987654321U);
          if ((res_h % 100) < 5) {
            Entity *res = calloc(1, sizeof(Entity));
            res->type = ENT_MOB;
            res->x = x;
            res->y = y;
            int pick = (res_h >> 8) % 3;
            res->id = ITEM_DB[pick].id;

            strncpy(res->name, ITEM_DB[pick].name, 31);

            cell->entity = res;
            da_append(&game->entities, res);
          }
        }
      }
    }
  }
}
