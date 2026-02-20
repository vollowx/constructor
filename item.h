#ifndef ITEM_H
#define ITEM_H

typedef enum {
  ITEM_RESOURCE,   // Ore, wood
  ITEM_CONSUMABLE, // Food, potions
  ITEM_EQUIPMENT,  // Tools, armor
  ITEM_PLACEABLE   // Seeds, furniture
} ItemType;

typedef struct {
  int id;
  ItemType type;
  char name[32];
  int max_stack;
  int weight;
} ItemDef;

typedef struct ItemStack {
  const ItemDef *def;
  int quantity;

  int durability; // Only used if def->type == ITEM_EQUIPMENT
} ItemStack;

char item_get_symbol(ItemDef item);
int item_get_color_pair(ItemDef item);

#endif
