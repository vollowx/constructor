#ifndef SAVE_H
#define SAVE_H

#include <stdint.h>

#include "game.h"

#define MAX_N_SLOTS 3

typedef enum {
  SAVE_OK,
  SAVE_ERR_OPEN,
  SAVE_ERR_READ,
  SAVE_ERR_WRITE,
  SAVE_ERR_VERSION
} SaveResult;

typedef struct {
  uint32_t magic;
  uint32_t version;
  char player_name[32];
  uint32_t timestamp;
} SaveHeader;

typedef struct {
  SaveHeader header;
  Game *game;
} Save;

SaveResult save_write(const Save *save, int slot);
SaveResult save_load(Save *save, int slot);
void save_init(Save *save, Game *game, const char *player_name);

typedef struct {
  SaveHeader header;
  bool exists;
} SavePreview;

SavePreview get_slot_preview(int slot);

#endif
