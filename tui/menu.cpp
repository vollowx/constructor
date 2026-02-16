#include "menu.h"

// TODO: Privide options to re-position the menu, and pre-provide some anchors
// like `centercenter`, `topleft`, `bottomright`, etc.

nco::menu::menu() { 
  _menu = new_menu(nullptr); 
  _window = nullptr;
  _selected_index = 0;
}

nco::menu::~menu() {
  if (_menu) {
    unpost_menu(_menu);
    free_menu(_menu);
  }
  for (ITEM *item : _items) {
    free_item(item);
  }
  if (_window) {
    delwin(_window);
  }
}

int nco::menu::set_mark(const char *mark) { return set_menu_mark(_menu, mark); }
const char *nco::menu::get_mark() { return menu_mark(_menu); }

int nco::menu::count() { return item_count(_menu); }

void nco::menu::add_item(const char *name, const char *desc,
                         std::function<void()> callback) {
  // if (_items.back() == nullptr) {
  //   _items.pop_back();
  // }

  ITEM *item = new_item(name, desc);

  _items.push_back(item);
  _callbacks.push_back(callback);

  // _items.push_back(nullptr);

  set_menu_items(_menu, _items.data());
}

void nco::menu::set_title(const std::string &title) { _title = title; }
const std::string &nco::menu::get_title() const { return _title; }

void nco::menu::render() {
  _window = newwin(12, 36, 0, 0);
  keypad(_window, TRUE);

  set_menu_win(_menu, _window);
  set_menu_sub(_menu, derwin(_window, 11, 36, 1, 0));

  attron(A_BOLD);
  mvwprintw(_window, 0, 0, " @ %s", _title.c_str());
  attroff(A_BOLD);
  refresh();

  post_menu(_menu);
  wrefresh(_window);

  int ch;
  bool end = false;

  while (!end) {
    ch = getch();

    switch (ch) {
    case KEY_DOWN:
    case 'j':
      menu_driver(_menu, REQ_DOWN_ITEM);
      wrefresh(_window);
      break;
    case KEY_UP:
    case 'k':
      menu_driver(_menu, REQ_UP_ITEM);
      wrefresh(_window);
      break;
    case 10: // Enter key
      end = true;
      // Save the selected index before unposting
      ITEM *current = current_item(_menu);
      _selected_index = item_index(current);
      break;
    }
  }
}

void nco::menu::exec_callback() {
  _callbacks[_selected_index]();
}
