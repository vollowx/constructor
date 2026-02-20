#ifndef MAP_H
#define MAP_H

#include <stdbool.h>
#include <stddef.h>

typedef enum {
  ELEV_DEEP_WATER,
  ELEV_WATER,
  ELEV_GROUND,
  ELEV_HILL,
  ELEV_MOUNTAIN
} Elevation;

typedef struct Map {
  size_t w, h;
  struct MapCell **cells;
} Map;

typedef struct MapCell {
  Elevation elevation;
  struct MapObject *object;
  struct Entity *entity;
} MapCell;

typedef struct MapObject {
  int id;
  int w, h, x, y;
  int health;
  bool is_passable;
} MapObject;

#endif
