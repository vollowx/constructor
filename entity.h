#ifndef ENTITY_H
#define ENTITY_H

#include "item.h"
#include "map.h"

typedef struct Inventory {
  int capacity;
  int count;
  ItemStack *slots;
} Inventory;

typedef enum { ENT_PLAYER, ENT_NPC, ENT_MOB, ENT_ITEM_DROP } EntityType;

typedef struct Entity {
  int id;
  EntityType type;
  char name[32];
  int health;
  int health_max;
  int x, y;
  Elevation z;

  struct Inventory inventory;
} Entity;

char entity_get_symbol(struct Entity entity);
int entity_get_color_pair(struct Entity entity);

#endif
