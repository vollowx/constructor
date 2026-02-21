#ifndef STATES_H
#define STATES_H

STATES_H
typedef enum {
  STATE_UNREACHABLE, // For always-on models like log
  STATE_QUIT,

  // States that change the current primary model
  STATE_MAIN_MENU,
  STATE_SAVES,
  STATE_GAMEPLAY,
  STATE_OPTIONS,
} GameState;

// Defined in main.c
extern GameState next_state;

#endif
