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
  uint32_t timestamp;
  char player_name[32];
} SaveHeader;

typedef struct {
  SaveHeader header;
  Game *game;
} Save;

typedef struct {
  SaveHeader header;
  bool exists;
} SavePreview;

SaveResult save_load(Save *save, int slot);
SaveResult save_save(const Save *save, int slot);
void save_init(Save *save);

SavePreview get_slot_preview(int slot);

#endif
