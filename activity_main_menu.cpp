#include "activities.h"
#include "tui/menu.h"
#include <menu.h>

bool activity_main_menu() {
  bool quit = false;

  nco::menu main_menu;

  main_menu.add_item("New", "Generate A New World",
                     [&]() -> void { /* activity_new_game(); */
                     });
  main_menu.add_item("Load", "Load A Saved World from Disk",
                     [&]() -> void { /* activity_load_game(); */ });
  main_menu.add_item("Settings", "Open Settings Page", [&]() -> void {
    while (activity_settings()) {
    };
  });
  main_menu.add_item("Quit", "Quit The Game", [&]() -> void { quit = true; });

  main_menu.build_menu();

  main_menu.render();

  return !quit;
}
