#include "tui/menu.h"
#include <menu.h>

bool _activity_ui_settings() {
  bool quit = false;

  nco::menu ui_settings_menu;
  ui_settings_menu.set_mark(" * ");
  ui_settings_menu.set_title("Settings / UI");

  ui_settings_menu.add_item("<--", "", [&]() -> void { quit = true; });
  ui_settings_menu.add_item("Color", "",
                            [&]() -> void { /* activity_new_game(); */
                            });
  ui_settings_menu.add_item("Language", "",
                            [&]() -> void { /* activity_settings(); */
                            });

  ui_settings_menu.render();
  ui_settings_menu.exec_callback();

  return !quit;
}

bool activity_settings() {
  bool quit = false;

  nco::menu settings_menu;
  settings_menu.set_mark(" * ");
  settings_menu.set_title("Settings");

  settings_menu.add_item("<--", "", [&]() -> void { quit = true; });
  settings_menu.add_item("UI", "", [&]() -> void {
    while (_activity_ui_settings()) {
    };
  });
  settings_menu.add_item("Control", "",
                         [&]() -> void { /* activity_load_game(); */ });

  settings_menu.render();
  settings_menu.exec_callback();

  return !quit;
}
