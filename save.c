#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "helpers.h"
#include "log.h"
#include "save.h"

#define SAVE_MAGIC 0x47414D45 // "GAME"
#define SAVE_VERSION 1

static void get_path(int slot, char *buf, size_t len) {
  snprintf(buf, len, "save%d.dat", slot);
}

static SaveResult map_write(const Map *map, FILE *fp) {

  if (fwrite(&map->w, sizeof(size_t), 1, fp) != 1)
    return SAVE_ERR_WRITE;
  if (fwrite(&map->h, sizeof(size_t), 1, fp) != 1)
    return SAVE_ERR_WRITE;

  for (size_t y = 0; y < map->h; ++y) {
    for (size_t x = 0; x < map->w; ++x) {
      // We write the flat cell data (Elevation).
      // Entity/Object pointers are ignored as they are transient/reconstructed.
      if (fwrite(&map->cells[y][x].elevation, sizeof(Elevation), 1, fp) != 1) {
        return SAVE_ERR_WRITE;
      }
    }
  }
  return SAVE_OK;
}

static SaveResult entity_write(const Entity *ent, FILE *fp) {
  // 1. Write the Entity struct
  if (fwrite(ent, sizeof(Entity), 1, fp) != 1)
    return SAVE_ERR_WRITE;

  // 2. Write Inventory items
  for (int i = 0; i < ent->inventory.count; ++i) {
    ItemStack *slot = &ent->inventory.items[i];

    // Convert pointer back to ID
    int def_id = slot->def ? slot->def->id : -1;

    if (fwrite(&def_id, sizeof(int), 1, fp) != 1)
      return SAVE_ERR_WRITE;
    if (fwrite(&slot->quantity, sizeof(int), 1, fp) != 1)
      return SAVE_ERR_WRITE;
    if (fwrite(&slot->durability, sizeof(int), 1, fp) != 1)
      return SAVE_ERR_WRITE;
  }

  return SAVE_OK;
}

static SaveResult game_write(const Game *game, FILE *fp) {
  if (map_write(game->map, fp) != SAVE_OK) {
    error("[save] Failed to write map data");
    return SAVE_ERR_WRITE;
  }

  uint64_t count = (uint64_t)game->entities.count;
  if (fwrite(&count, sizeof(uint64_t), 1, fp) != 1)
    return SAVE_ERR_WRITE;

  da_foreach(Entity *, it, &game->entities) {
    if (entity_write(*it, fp) != SAVE_OK) {

      error("[save] Failed to write entity '%s'", (*it)->name);
      return SAVE_ERR_WRITE;
    }
  }

  return SAVE_OK;
}

SaveResult save_write(const Save *self, int slot) {
  char path[128];
  get_path(slot, path, sizeof(path));

  info("[save] Writing save to %s...", path);

  FILE *fp = fopen(path, "wb");
  if (!fp) {
    error("[save] Could not open %s for writing", path);
    return SAVE_ERR_OPEN;
  }

  if (fwrite(&self->header, sizeof(SaveHeader), 1, fp) != 1) {
    fclose(fp);
    return SAVE_ERR_WRITE;
  }

  SaveResult res = game_write(self->game, fp);

  fclose(fp);
  if (res == SAVE_OK)
    info("[save] Wrote to %s", path);

  return res;
}

static SaveResult map_read(Map *map, FILE *fp) {
  size_t w, h;
  if (fread(&w, sizeof(size_t), 1, fp) != 1)
    return SAVE_ERR_READ;
  if (fread(&h, sizeof(size_t), 1, fp) != 1)
    return SAVE_ERR_READ;

  // Use the proper engine functions to clear and rebuild the map
  if (map) {
    free_map(map);
  }
  map = new_map(h, w);
  if (!map)
    return SAVE_ERR_READ;

  for (size_t y = 0; y < h; ++y) {
    for (size_t x = 0; x < w; ++x) {
      if (fread(&map->cells[y][x].elevation, sizeof(Elevation), 1, fp) != 1) {
        return SAVE_ERR_READ;
      }
      // Pointers safely zeroed inside new_map, but explicit is good:
      map->cells[y][x].entity = NULL;
      map->cells[y][x].object = NULL;
    }
  }
  return SAVE_OK;
}

SaveResult save_load(Save *self, int slot) {
  char path[128];
  get_path(slot, path, sizeof(path));

  info("[save] Loading game from %s...", path);

  FILE *fp = fopen(path, "rb");
  if (!fp) {
    error("[save] Could not open %s for reading", path);
    return SAVE_ERR_OPEN;
  }

  if (fread(&self->header, sizeof(SaveHeader), 1, fp) != 1) {
    error("[save] Failed to read header");
    fclose(fp);
    return SAVE_ERR_READ;
  }

  if (self->header.magic != SAVE_MAGIC) {
    error("[save] Corrupt save (invalid header)");
    fclose(fp);
    return SAVE_ERR_VERSION;
  }
  if (self->header.version != SAVE_VERSION) {
    error("[save] Version mismatch (expected %d, got %d)", SAVE_VERSION,
          self->header.version);
    fclose(fp);
    return SAVE_ERR_VERSION;
  }

  info("[save] Header valid with player: %s", self->header.player_name);

  // Read Map
  if (map_read(self->game->map, fp) != SAVE_OK) {
    error("[save] Failed reading map");
    fclose(fp);
    return SAVE_ERR_READ;
  }

  // Read Entities
  uint64_t entity_count;

  if (fread(&entity_count, sizeof(uint64_t), 1, fp) != 1) {
    fclose(fp);
    return SAVE_ERR_READ;
  }

  info("[save] Loading %llu entities...", (unsigned long long)entity_count);

  // NOTE: You must ensure self->game->entities is freed/cleared before this
  // point if you are loading a game while already playing!
  da_reserve(&self->game->entities, (size_t)entity_count);
  self->game->player = NULL;

  for (size_t i = 0; i < entity_count; ++i) {
    Entity *ent = malloc(sizeof(Entity));
    if (fread(ent, sizeof(Entity), 1, fp) != 1) {
      error("[save] Failed to read entity %zu", i);
      free(ent);
      fclose(fp);
      return SAVE_ERR_READ;
    }

    if (ent->inventory.count > 0) {
      ent->inventory.items =
          malloc(sizeof(ItemStack) * ent->inventory.capacity);
      for (int j = 0; j < ent->inventory.count; ++j) {
        int def_id;

        if (fread(&def_id, sizeof(int), 1, fp) != 1 ||
            fread(&ent->inventory.items[j].quantity, sizeof(int), 1, fp) != 1 ||
            fread(&ent->inventory.items[j].durability, sizeof(int), 1, fp) !=
                1) {

          error("[save] Failed to read inventory slot %d for entity '%s'", j,
                ent->name);
          fclose(fp);
          return SAVE_ERR_READ;
        }
        ent->inventory.items[j].def = item_get_def(def_id);
      }

    } else {
      ent->inventory.items = NULL;
      ent->inventory.capacity = 0;
    }

    da_append(&self->game->entities, ent);

    // Re-hydrate Map Pointers (With Bounds Check)

    if (ent->x >= 0 && (size_t)ent->x < self->game->map->w && ent->y >= 0 &&
        (size_t)ent->y < self->game->map->h) {
      self->game->map->cells[ent->y][ent->x].entity = ent;

    } else {
      error("[save] Entity '%s' loaded out of map bounds! (%d, %d)", ent->name,
            ent->x, ent->y);
    }

    if (ent->type == ENT_PLAYER) {
      self->game->player = ent;
    }
  }

  fclose(fp);
  info("[save] Loaded successfully");
  return SAVE_OK;
}

void save_init(Save *self, Game *game, const char *player_name) {
  self->game = game;

  self->header.magic = SAVE_MAGIC;
  self->header.version = SAVE_VERSION;
  self->header.timestamp = (uint32_t)time(NULL);

  memset(self->header.player_name, 0, sizeof(self->header.player_name));
  if (player_name) {
    strncpy(self->header.player_name, player_name,
            sizeof(self->header.player_name) - 1);
    if (game->player) {
      strncpy(game->player->name, player_name, sizeof(game->player->name) - 1);
    }
  }
}

SavePreview get_slot_preview(int slot) {
  SavePreview p = {0};

  char path[128];
  get_path(slot, path, sizeof(path));

  FILE *fp = fopen(path, "rb");
  if (fp) {
    if (fread(&p.header, sizeof(SaveHeader), 1, fp) == 1) {
      p.exists = (p.header.magic == SAVE_MAGIC);
    }
    fclose(fp);
  }

  return p;
}
