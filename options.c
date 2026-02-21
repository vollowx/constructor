#include <stdio.h>
#include <string.h>

#include "log.h"
#include "options.h"

#define PATH_OPTIONS "options.txt"

GameOptions current_options = {.log_level = 0, .show_log = true};

void options_load() {
  FILE *f = fopen(PATH_OPTIONS, "r");

  if (!f) {
    log_add(LOG_INFO, "options.txt not found, using default options");
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

  log_add(LOG_INFO, "options loaded");
}

void options_write() {
  FILE *f = fopen(PATH_OPTIONS, "w");

  if (!f)
    return;
  fprintf(f, "log_level=%d\n", current_options.log_level);
  fprintf(f, "save_log=%d\n", (int)current_options.save_log);
  fprintf(f, "show_log=%d\n", (int)current_options.show_log);
  fclose(f);

  log_add(LOG_INFO, "options saved");
}
