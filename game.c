#include "game.h"
#include "helpers.h"

static const ItemDef ITEM_DATABASE[] = {
    {.id = 0, .name = "Iron Ore", .type = ITEM_RESOURCE, .max_stack = 64},
    {.id = 1, .name = "Apple", .type = ITEM_CONSUMABLE, .max_stack = 20},
    {.id = 2, .name = "Pickaxe", .type = ITEM_EQUIPMENT, .max_stack = 1}};

const ItemDef *item_get_def(int id) {
  for (size_t i = 0; i < sizeof(ITEM_DATABASE) / sizeof(ItemDef); ++i) {
    if (ITEM_DATABASE[i].id == id)
      return &ITEM_DATABASE[i];
  }
  return NULL;
}

void entity_move(Entity *e, int x, int y, Map *map) {
  map->cells[e->y][e->x].entity = NULL;

  e->x = x;
  e->y = y;
  e->z = map->cells[y][x].elevation;

  map->cells[y][x].entity = e;
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

  // 1. Free each row of cells
  for (size_t y = 0; y < map->h; ++y) {
    free(map->cells[y]);
  }

  // 2. Free the row pointer array
  free(map->cells);

  // 3. Free the Map container itself
  free(map);
}

void game_init(Game *game) {
  // 1. Initialize the Map
  game->map = new_map(80, 24);

  // 2. Initialize the Entities container
  game->entities.items = NULL;
  game->entities.count = 0;
  game->entities.capacity = 0;
  da_reserve(&game->entities, NOB_DA_INIT_CAP);

  // 3. Create the Player
  Entity *player = malloc(sizeof(Entity));
  memset(player, 0, sizeof(Entity));

  player->id = 0;
  player->type = ENT_PLAYER;
  player->health = 100;
  player->health_max = 100;
  player->x = 5;
  player->y = 5;
  player->z = ELEV_GROUND;
  strncpy(player->name, "Hero", sizeof(player->name) - 1);

  // Initialize Player Inventory
  player->inventory.count = 0;
  player->inventory.capacity = 8;
  player->inventory.items =
      malloc(sizeof(ItemStack) * player->inventory.capacity);

  // 4. Register Player in the Game
  da_append(&game->entities, player);
  game->player = player;

  // 5. Link Player to the Map grid
  game->map->cells[player->y][player->x].entity = player;
}
