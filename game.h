#ifndef GAME_H
#define GAME_H

#include "entity.h"
#include "map.h"

typedef struct {
  Map *map;
  Entity *player;
  Entity **mobs;
  int mob_count;
} ModelGame;

#endif
