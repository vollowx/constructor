#include "activities.h"
#include "tui/menu.h"
#include <menu.h>

bool activity_main_menu() {
  bool quit = false;

  nco::menu main_menu;
  main_menu.set_mark(" * ");
  main_menu.set_title("Constructor");

  main_menu.add_item("New", "", [&]() -> void { activity_new_game(); });
  main_menu.add_item("Load", "", [&]() -> void { activity_load_game(); });
  main_menu.add_item("Settings", "", [&]() -> void {
    while (activity_settings()) {
    };
  });
  main_menu.add_item("Quit", "", [&]() -> void { quit = true; });

  main_menu.render();
  main_menu.exec_callback();

  return !quit;
}
