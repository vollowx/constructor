#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "game.h"
#include "helpers.h"
#include "log.h"
#include "save.h"

static void get_path(int slot, char *buf, size_t len) {
  snprintf(buf, len, "save%d.dat", slot);
}

static SaveResult map_load(Map **map_ptr, FILE *fp) {
  size_t w, h;
  if (fread(&w, sizeof(size_t), 1, fp) != 1)
    return SAVE_ERR_READ;
  if (fread(&h, sizeof(size_t), 1, fp) != 1)
    return SAVE_ERR_READ;

  if (*map_ptr) {
    free_map(*map_ptr);
  }
  *map_ptr = new_map(h, w);
  Map *map = *map_ptr;

  if (!map_ptr)
    return SAVE_ERR_READ;

  for (size_t y = 0; y < h; ++y) {
    for (size_t x = 0; x < w; ++x) {
      MapCell *cell = &map->cells[y][x];

      if (fread(&cell->elevation, sizeof(Elevation), 1, fp) != 1)
        return SAVE_ERR_READ;
      if (fread(&cell->object_id, sizeof(uint16_t), 1, fp) != 1)
        return SAVE_ERR_READ;
      if (fread(&cell->object_health, sizeof(int), 1, fp) != 1)
        return SAVE_ERR_READ;
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

  if (map_load(&self->game->map, fp) != SAVE_OK) {
    error("[save] Failed to load map");
    fclose(fp);
    return SAVE_ERR_READ;
  }

  size_t entity_count;

  if (fread(&entity_count, sizeof(size_t), 1, fp) != 1) {
    fclose(fp);
    return SAVE_ERR_READ;
  }

  info("[save] Loading %llu entities...", (unsigned long long)entity_count);

  // NOTE: You must ensure self->game->entities is freed/cleared before this
  // point if you are loading a game while already playing!
  da_reserve(&self->game->entities, entity_count);
  self->game->player = NULL;

  for (size_t i = 0; i < entity_count; ++i) {
    Entity *ent = malloc(sizeof(Entity));
    uint16_t def_id;

    if (fread(&def_id, sizeof(uint16_t), 1, fp) != 1) {
      free(ent);
      fclose(fp);
      return SAVE_ERR_READ;
    }
    ent->def = entity_get_def(def_id);

    if (fread(ent->name, sizeof(char), 32, fp) != 32) {
      free(ent);
      fclose(fp);
      return SAVE_ERR_READ;
    }
    if (fread(&ent->health, sizeof(int), 1, fp) != 1) {
      free(ent);
      fclose(fp);
      return SAVE_ERR_READ;
    }
    if (fread(&ent->x, sizeof(size_t), 1, fp) != 1) {
      free(ent);
      fclose(fp);
      return SAVE_ERR_READ;
    }
    if (fread(&ent->y, sizeof(size_t), 1, fp) != 1) {
      free(ent);
      fclose(fp);
      return SAVE_ERR_READ;
    }

    if (fread(&ent->inventory.capacity, sizeof(int), 1, fp) != 1) {
      free(ent);
      fclose(fp);
      return SAVE_ERR_READ;
    }
    if (fread(&ent->inventory.count, sizeof(int), 1, fp) != 1) {
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

          error("[save] Failed to load inventory slot %d for entity '%s'", j,
                ent->name);
          fclose(fp);
          return SAVE_ERR_READ;
        }
        ent->inventory.items[j].def = item_get_def(def_id);
      }
    } else {
      ent->inventory.items = NULL;
    }

    da_append(&self->game->entities, ent);

    if (ent->x < self->game->map->w && ent->y < self->game->map->h) {
      self->game->map->cells[ent->y][ent->x].entity = ent;
    } else {
      error("[save] Entity '%s' is out of map bounds (%d, %d)", ent->name,
            ent->x, ent->y);
    }

    if (ent->def->type == ENTITY_PLAYER) {
      self->game->player = ent;
    }
  }

  fclose(fp);
  info("[save] Loaded successfully", path);
  return SAVE_OK;
}

static SaveResult map_save(const Map *map, FILE *fp) {
  if (fwrite(&map->w, sizeof(size_t), 1, fp) != 1)
    return SAVE_ERR_WRITE;
  if (fwrite(&map->h, sizeof(size_t), 1, fp) != 1)
    return SAVE_ERR_WRITE;

  for (size_t y = 0; y < map->h; ++y) {
    for (size_t x = 0; x < map->w; ++x) {
      MapCell *cell = &map->cells[y][x];
      if (fwrite(&cell->elevation, sizeof(Elevation), 1, fp) != 1)
        return SAVE_ERR_WRITE;
      if (fwrite(&cell->object_id, sizeof(uint16_t), 1, fp) != 1)
        return SAVE_ERR_WRITE;
      if (fwrite(&cell->object_health, sizeof(int), 1, fp) != 1)
        return SAVE_ERR_WRITE;
    }
  }
  return SAVE_OK;
}

static SaveResult entity_save(const Entity *ent, FILE *fp) {
  uint16_t def_id = ent->def->id;

  if (fwrite(&def_id, sizeof(uint16_t), 1, fp) != 1)
    return SAVE_ERR_WRITE;
  if (fwrite(ent->name, sizeof(char), 32, fp) != 32)
    return SAVE_ERR_WRITE;
  if (fwrite(&ent->health, sizeof(int), 1, fp) != 1)
    return SAVE_ERR_WRITE;
  if (fwrite(&ent->x, sizeof(size_t), 1, fp) != 1)
    return SAVE_ERR_WRITE;
  if (fwrite(&ent->y, sizeof(size_t), 1, fp) != 1)
    return SAVE_ERR_WRITE;

  if (fwrite(&ent->inventory.capacity, sizeof(int), 1, fp) != 1)
    return SAVE_ERR_WRITE;
  if (fwrite(&ent->inventory.count, sizeof(int), 1, fp) != 1)
    return SAVE_ERR_WRITE;

  for (int i = 0; i < ent->inventory.count; ++i) {
    ItemStack *slot = &ent->inventory.items[i];
    uint16_t item_def_id = slot->def ? slot->def->id : 0;

    if (fwrite(&item_def_id, sizeof(int), 1, fp) != 1)
      return SAVE_ERR_WRITE;
    if (fwrite(&slot->quantity, sizeof(int), 1, fp) != 1)
      return SAVE_ERR_WRITE;
    if (fwrite(&slot->durability, sizeof(int), 1, fp) != 1)
      return SAVE_ERR_WRITE;
  }

  return SAVE_OK;
}

static SaveResult game_save(const Game *game, FILE *fp) {
  if (map_save(game->map, fp) != SAVE_OK) {
    error("[save] Failed to write map");
    return SAVE_ERR_WRITE;
  }

  size_t count = game->entities.count;
  if (fwrite(&count, sizeof(size_t), 1, fp) != 1)
    return SAVE_ERR_WRITE;

  da_foreach(Entity *, it, &game->entities) {
    if (entity_save(*it, fp) != SAVE_OK) {
      error("[save] Failed to write entity '%s'", (*it)->name);
      return SAVE_ERR_WRITE;
    }
  }

  return SAVE_OK;
}

SaveResult save_save(const Save *self, int slot) {
  char path[128];
  get_path(slot, path, sizeof(path));

  info("[save] Saving to %s...", path);

  FILE *fp = fopen(path, "wb");
  if (!fp) {
    error("[save] Could not open %s for writing", path);
    return SAVE_ERR_OPEN;
  }

  if (fwrite(&self->header, sizeof(SaveHeader), 1, fp) != 1) {
    fclose(fp);
    return SAVE_ERR_WRITE;
  }

  SaveResult res = game_save(self->game, fp);

  fclose(fp);
  if (res == SAVE_OK)
    info("[save] Saved successfully", path);

  return res;
}

void save_init(Save *self) {
  self->header.magic = SAVE_MAGIC;
  self->header.version = SAVE_VERSION;
  self->header.timestamp = (uint32_t)time(NULL);
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
