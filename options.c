#include <stdio.h>
#include <string.h>

#include "log.h"
#include "options.h"

#define PATH_OPTIONS "options.txt"

GameOptions current_options = {.log_level = 0, .show_log = true};

void options_load() {
  FILE *f = fopen(PATH_OPTIONS, "r");

  if (!f) {
    warn("[options] %s not found, falling back to default options", PATH_OPTIONS);
    return;
  }
  char key[32];
  int val;
  while (fscanf(f, "%31[^=]=%d\n", key, &val) == 2) {
    if (strcmp(key, "log_level") == 0)
      current_options.log_level = val;
    else if (strcmp(key, "save_log") == 0)
      current_options.save_log = (bool)val;
    else if (strcmp(key, "show_log") == 0)
      current_options.show_log = (bool)val;
  }
  fclose(f);

  info("[options] loaded from %s", PATH_OPTIONS);
}

void options_save() {
  FILE *f = fopen(PATH_OPTIONS, "w");

  if (!f)
    return;
  fprintf(f, "log_level=%d\n", current_options.log_level);
  fprintf(f, "save_log=%d\n", (int)current_options.save_log);
  fprintf(f, "show_log=%d\n", (int)current_options.show_log);
  fclose(f);

  info("[options] wrote to %s", PATH_OPTIONS);
}
