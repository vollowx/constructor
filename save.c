#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "helpers.h"
#include "save.h"

static void get_path(int slot, char *buf, size_t len) {
  snprintf(buf, len, "save_%d.dat", slot);
}

static SaveResult map_write(const Map *map, FILE *fp) {
  if (fwrite(&map->w, sizeof(size_t), 1, fp) != 1)
    return SAVE_ERR_WRITE;
  if (fwrite(&map->h, sizeof(size_t), 1, fp) != 1)
    return SAVE_ERR_WRITE;

  int result;

  for (size_t y = 0; y < map->h; ++y) {
    for (size_t x = 0; x < map->w; ++x) {
      // We write the flat cell data (Elevation).
      // Entity/Object pointers are ignored as they are transient.
      result = fwrite(&map->cells[y][x].elevation, sizeof(Elevation), 1, fp);
      if (result != 1)
        return SAVE_ERR_WRITE;
    }
  }

  return SAVE_OK;
}

static SaveResult entity_write(const Entity *ent, FILE *fp) {
  // 1. Write the Entity struct (contains health, x, y, etc.)
  if (fwrite(ent, sizeof(Entity), 1, fp) != 1)
    return SAVE_ERR_WRITE;

  // 2. Write Inventory items individually to handle the pointer
  for (int i = 0; i < ent->inventory.count; ++i) {
    ItemStack *slot = &ent->inventory.items[i];

    // Write the ID of the definition instead of the pointer

    int def_id = slot->def ? slot->def->id : -1;
    fwrite(&def_id, sizeof(int), 1, fp);

    // Write the rest of the stack (quantity, durability)

    fwrite(&slot->quantity, sizeof(int), 1, fp);
    fwrite(&slot->durability, sizeof(int), 1, fp);
  }
  return SAVE_OK;
}

static SaveResult game_write(const Game *game, FILE *fp) {
  if (map_write(game->map, fp) != SAVE_OK)
    return SAVE_ERR_WRITE;

  // Write entity count for the loop in load
  uint64_t count = (uint64_t)game->entities.count;

  fwrite(&count, sizeof(uint64_t), 1, fp);

  // Use your nob_da_foreach alias
  da_foreach(Entity *, it, &game->entities) {
    if (entity_write(*it, fp) != SAVE_OK)
      return SAVE_ERR_WRITE;
  }

  return SAVE_OK;
}

SaveResult save_write(const Save *self, int slot) {
  char path[128];
  get_path(slot, path, sizeof(path));

  FILE *fp = fopen(path, "wb");
  if (!fp)
    return SAVE_ERR_OPEN;

  // Write Header (Magic, Version, etc.)
  fwrite(&self->header, sizeof(SaveHeader), 1, fp);

  SaveResult res = game_write(self->game, fp);

  fclose(fp);

  return res;
}

static SaveResult map_read(Map *map, FILE *fp) {
  if (fread(&map->w, sizeof(size_t), 1, fp) != 1)
    return SAVE_ERR_READ;
  if (fread(&map->h, sizeof(size_t), 1, fp) != 1)
    return SAVE_ERR_READ;

  // Allocate the row pointers
  map->cells = malloc(sizeof(MapCell *) * map->h);
  for (size_t y = 0; y < map->h; ++y) {
    map->cells[y] = malloc(sizeof(MapCell) * map->w);

    for (size_t x = 0; x < map->w; ++x) {

      fread(&map->cells[y][x].elevation, sizeof(Elevation), 1, fp);
      // Clear pointers to avoid pointing to garbage memory
      map->cells[y][x].entity = NULL;
      map->cells[y][x].object = NULL;
    }
  }
  return SAVE_OK;
}

SaveResult save_load(Save *self, int slot) {
  char path[128];
  get_path(slot, path, sizeof(path));

  FILE *fp = fopen(path, "rb");

  if (!fp)
    return SAVE_ERR_OPEN;

  fread(&self->header, sizeof(SaveHeader), 1, fp);
  // ... magic/version checks ...

  map_read(self->game->map, fp);

  uint64_t entity_count;
  fread(&entity_count, sizeof(uint64_t), 1, fp);

  // Use nob_da_reserve to allocate memory once
  da_reserve(&self->game->entities, (size_t)entity_count);

  for (size_t i = 0; i < entity_count; ++i) {

    Entity *ent = malloc(sizeof(Entity));
    fread(ent, sizeof(Entity), 1, fp);

    if (ent->inventory.count > 0) {
      ent->inventory.items =
          malloc(sizeof(ItemStack) * ent->inventory.capacity);

      for (int j = 0; j < ent->inventory.count; ++j) {
        int def_id;
        fread(&def_id, sizeof(int), 1, fp);

        // Re-link the pointer using our lookup function
        ent->inventory.items[j].def = item_get_def(def_id);

        fread(&ent->inventory.items[j].quantity, sizeof(int), 1, fp);
        fread(&ent->inventory.items[j].durability, sizeof(int), 1, fp);
      }
    }

    da_append(&self->game->entities, ent);

    // Re-hydrate Map Pointers: Linking the MapCell back to this Entity
    self->game->map->cells[ent->y][ent->x].entity = ent;

    // Set player pointer if this entity is the player
    if (ent->type == ENT_PLAYER) {
      self->game->player = ent;
    }
  }

  fclose(fp);
  return SAVE_OK;
}

void save_init(Save *self, Game *game, const char *player_name) {
  self->game = game;

  self->header.magic = 0x47414D45;
  self->header.version = 1;
  self->header.timestamp = (uint32_t)time(NULL);

  memset(self->header.player_name, 0, sizeof(self->header.player_name));
  if (player_name) {
    strncpy(self->header.player_name, player_name,
            sizeof(self->header.player_name) - 1);
    if (game->player)
      strncpy(game->player->name, player_name, sizeof(game->player->name) - 1);
  }
}

SavePreview get_slot_preview(int slot) {
  SavePreview p = {0};

  char path[128];
  get_path(slot, path, sizeof(path));

  FILE *fp = fopen(path, "rb");
  if (fp) {
    fread(&p.header, sizeof(SaveHeader), 1, fp);
    p.exists = (p.header.magic == 0x47414D45);
    fclose(fp);
  }

  return p;
}
