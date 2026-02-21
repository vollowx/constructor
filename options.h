#ifndef OPTIONS_H
#define OPTIONS_H

#include <stdbool.h>

typedef struct {
  int log_level;
  bool save_log;
  bool show_log;
} GameOptions;

extern GameOptions current_options;

void options_load();
void options_write();

#endif
