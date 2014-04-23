#include <pebble.h>
#include "windows/win-main.h"

static void init(void) {
  create_main_window();
  show_main_window(true);
}

static void deinit(void) {
  // Cleanup code here
  destroy_main_window();
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}
