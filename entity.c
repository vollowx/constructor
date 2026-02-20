#include "entity.h"
#include "map.h"

void move_entity(Entity *e, int new_x, int new_y, Map *map) {
  map->cells[e->y][e->x].entity = NULL;

  e->x = new_x;
  e->y = new_y;
  e->z = map->cells[new_y][new_x].elevation;

  map->cells[new_y][new_x].entity = e;
}
