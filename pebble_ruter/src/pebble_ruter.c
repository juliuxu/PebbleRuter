#include <pebble.h>
#include "pebble_ruter.h"
#include "windows/win-main.h"

const transport_type_t transport_types[NUM_TRANSPORT_TYPES] = {
  BUS,
  FERRY,
  TRAIN,
  TRAM,
  METRO,
};

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
