#ifndef GAME_H
#define GAME_H

#include <stdbool.h>
#include <stdlib.h>

// Order:
// - item
// - entity
// - map
// - game

typedef enum {
  ELEV_DEEP_WATER,
  ELEV_WATER,
  ELEV_GROUND,
  ELEV_HILL,
  ELEV_MOUNTAIN
} Elevation;

typedef enum {
  ITEM_RESOURCE,   // Ore, wood
  ITEM_CONSUMABLE, // Food, potions
  ITEM_EQUIPMENT,  // Tools, armor
  ITEM_PLACEABLE   // Seeds, furniture
} ItemType;

typedef enum {
  ENT_PLAYER,
  ENT_NPC,
  ENT_MOB,
  ENT_ITEM_DROP,
} EntityType;

typedef struct {
  int id;
  ItemType type;
  char name[32];
  int max_stack;
  int weight;
} ItemDef;

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
  int x, y;
  Elevation z;

  ItemStacks inventory;
} Entity;

typedef struct {
  Entity **items;
  size_t count;
  size_t capacity;
} Entities;

typedef struct {
  int id;
  int w, h, x, y;
  int health;
  bool is_passable;
} MapObject;

typedef struct {
  Elevation elevation;
  MapObject *object;
  Entity *entity;
} MapCell;

typedef struct {
  size_t w, h;
  MapCell **cells;
} Map;

typedef struct {
  Map *map;

  Entity *player;
  Entities entities;
} Game;

const ItemDef *item_get_def(int id);
char item_get_symbol(ItemDef item);
int item_get_color_pair(ItemDef item);

char entity_get_symbol(Entity entity);
int entity_get_color_pair(Entity entity);
void entity_move(Entity *e, int x, int y, Map *map);

Map *new_map(size_t height, size_t width);
void free_map(Map *map);

void free_game(Game *game);
void game_init(Game *game);
void game_tick(Game *game);

#endif
