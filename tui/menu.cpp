#include "menu.h"

// TODO: Privide options to re-position the menu, and pre-provide some anchors
// like `centercenter`, `topleft`, `bottomright`, etc.

nco::menu::menu() { _menu = new_menu(nullptr); }

nco::menu::~menu() {
  unpost_menu(_menu);
  free_menu(_menu);
  for (ITEM *item : items) {
    free_item(item);
  }
}

void nco::menu::add_item(const char *name, const char *desc,
                         std::function<void()> callback) {
  ITEM *item = new_item(name, desc);
  items.push_back(item);
  callbacks.push_back(callback);
}

void nco::menu::build_menu() {
  items.push_back(nullptr);
  set_menu_items(_menu, items.data());
}

void nco::menu::render() {
  post_menu(_menu);
  refresh();

  int choice = 0;
  int ch;

  while ((ch = getch()) != 10 /* Enter key */) {
    switch (ch) {
    case KEY_DOWN:
    case 'j':
      menu_driver(_menu, REQ_DOWN_ITEM);
      break;
    case KEY_UP:
    case 'k':
      menu_driver(_menu, REQ_UP_ITEM);
      break;
    }
  }

  ITEM *index = current_item(_menu);
  choice = item_index(index);

  unpost_menu(_menu);
  // free_menu(_menu);
  // This causes a double free error. The menu is already freed
  // in the destructor in the class?

  callbacks[choice](); // Call the callback function for the selected item
}
