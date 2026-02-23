#ifndef GAME_H
#define GAME_H

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

// Order:
// 1. item
// 2. entity
// 3. object
// 4. map
// 5. game

typedef enum {
  ELEV_NONE = 0,
  ELEV_DEEP_WATER,
  ELEV_WATER,
  ELEV_GROUND,
  ELEV_HILL,
  ELEV_MOUNTAIN
} Elevation;

typedef enum {
  ITEM_RESOURCE,   // Ore, wood
  ITEM_PLACEABLE,  // Seeds, furniture
  ITEM_CONSUMABLE, // Food, potions
  ITEM_EQUIPMENT,  // Tools, armor
} ItemType;

typedef enum {
  ENT_PLAYER,
  ENT_NPC,
  ENT_MOB,
  ENT_ITEM_DROP,
} EntityType;

typedef struct {
  uint16_t id;
  ItemType type;
  char name[32];
  int max_stack;
  char symbol;
  short fg, bg;
} ItemDef;

typedef struct {
  uint16_t id;
  char name[32];
  int max_health;
  bool is_passable;
  char symbol;
  short fg, bg;
} ObjectDef;

typedef struct {
  const ItemDef *def;
  int quantity;
  int durability; // Only used if def->type == ITEM_EQUIPMENT
} ItemStack;

typedef struct {
  ItemStack *items;
  int capacity;
  int count;
} ItemStacks;

typedef struct {
  int id;
  EntityType type;
  char name[32];
  int health;
  int health_max;
  size_t x, y;
  Elevation z;

  ItemStacks inventory;
} Entity;

typedef struct {
  Entity **items;
  size_t count;
  size_t capacity;
} Entities;

typedef struct {
  uint16_t id;
  int health;
} Object;

typedef struct {
  Elevation elevation;
  Object *object;
  Entity *entity;
} MapCell;

typedef struct {
  size_t w, h;
  MapCell **cells;
} Map;

typedef struct {
  uint32_t seed;

  Map *map;
  Entity *player;
  Entities entities;
} Game;

extern const ItemDef ITEM_DB[];
// extern const EntityDef ENT_DB[];
extern const ObjectDef OBJ_DB[];

const ItemDef *item_get_def(int id);
char item_get_symbol(int id);

char entity_get_symbol(Entity entity);
void entity_move(Entity *e, int dx, int dy, Map *map);

Map *new_map(size_t height, size_t width);
void free_map(Map *map);

void free_game(Game *game);
void game_init(Game *game);
void game_gen_area(Game *game, size_t start_y, size_t start_x, size_t end_y,
                   size_t end_x);
void game_tick(Game *game);

#endif
