#ifndef GAME_H
#define GAME_H

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#include <ncurses.h>

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
  ELEV_MOUNTAIN,
  _elevation_count,
} Elevation;

typedef enum {
  ITEM_RESOURCE,   // Ore, wood
  ITEM_PLACEABLE,  // Seeds, furniture
  ITEM_CONSUMABLE, // Food, potions
  ITEM_EQUIPMENT,  // Tools, armor
} ItemType;

typedef enum {
  ENTITY_PLAYER,
  ENTITY_NPC,
  ENTITY_ENEMY,
  ENTITY_ANIMAL,
  ENTITY_ITEM,
} EntityType;

typedef struct {
  uint16_t id;
  ItemType type;
  char name[32];
  int max_stack;

  char symbol[2];
  short fg, bg;
  attr_t attr;
} ItemDef;

typedef struct {
  uint16_t id;
  EntityType type;
  char name[32];
  int max_health;
  bool is_passable;

  char symbol[2];
  short fg, bg;
  attr_t attr;
} EntityDef;

typedef struct {
  uint16_t id;
  char name[32];
  int max_health;
  bool is_passable;

  char symbol[2];
  short fg, bg;
  attr_t attr;
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
  const EntityDef *def;
  char name[32];
  int health;
  size_t x, y;

  ItemStacks inventory;
} Entity;

typedef struct {
  Entity **items;
  size_t count;
  size_t capacity;
} Entities;

typedef struct {
  Elevation elevation;
  uint16_t object_id;
  int object_health;
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

const ItemDef *item_get_def(int id);
const EntityDef *entity_get_def(int id);
const ObjectDef *object_get_def(int id);

bool entity_move(Entity *e, Map *map, int dx, int dy);
bool entity_place_object(Entity *e, Map *map, uint16_t object_id, int dx,
                         int dy);

Map *new_map(size_t height, size_t width);
void free_map(Map *map);

void free_game(Game *game);
void game_init(Game *game);
void game_gen_area(Game *game, size_t start_y, size_t start_x, size_t end_y,
                   size_t end_x);
bool game_tick(Game *game, double dt);

#endif
