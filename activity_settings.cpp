#include "tui/menu.h"
#include <menu.h>

bool _activity_ui_settings() {
  bool quit = false;

  nco::menu ui_settings_menu;

  ui_settings_menu.add_item("<--", "Go to Previous Page",
                            [&]() -> void { quit = true; });
  ui_settings_menu.add_item("Color", "Color Settings",
                            [&]() -> void { /* activity_new_game(); */
                            });
  ui_settings_menu.add_item("Language", "Language Settings",
                            [&]() -> void { /* activity_settings(); */
                            });

  ui_settings_menu.build_menu();

  ui_settings_menu.render();

  return !quit;
}

bool activity_settings() {
  bool quit = false;

  nco::menu settings_menu;

  settings_menu.add_item("<--", "Go to Previous Page",
                         [&]() -> void { quit = true; });
  settings_menu.add_item("UI", "User Interface Settings", [&]() -> void {
    while (_activity_ui_settings()) {
    };
  });
  settings_menu.add_item("Control", "Load A Saved World from Disk",
                         [&]() -> void { /* activity_load_game(); */ });

  settings_menu.build_menu();

  settings_menu.render();

  return !quit;
}
