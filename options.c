#include <stdio.h>
#include <string.h>

#include "log.h"
#include "options.h"

GameOptions current_options = {.log_level = 0, .show_log = true};

void options_load() {
  FILE *f = fopen("options.txt", "r");

  if (!f) {
    log_add(LOG_INFO, "options.txt not found, using default options");
    return;
  }
  char key[32];
  int val;
  while (fscanf(f, "%31[^=]=%d\n", key, &val) == 2) {
    if (strcmp(key, "log_level") == 0)
      current_options.log_level = val;
    else if (strcmp(key, "show_log") == 0)
      current_options.show_log = (bool)val;
  }
  fclose(f);

  log_add(LOG_INFO, "options loaded");
}

void options_save() {
  FILE *f = fopen("options.txt", "w");

  if (!f)
    return;
  fprintf(f, "log_level=%d\n", current_options.log_level);
  fprintf(f, "show_log=%d\n", (int)current_options.show_log);
  fclose(f);

  log_add(LOG_INFO, "options saved");
}
