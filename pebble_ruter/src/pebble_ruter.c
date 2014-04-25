#include <pebble.h>
#include "pebble_ruter.h"
#include "windows/win-main.h"

const realtime_transport_type_t realtime_transport_types[NUM_REALTIME_TRANSPORT_TYPES] = {
  R_BUS,
  R_FERRY,
  R_TRAIN,
  R_TRAM,
  R_METRO,
};

const travel_transport_type_t travel_transport_types[NUM_TRAVEL_TRANSPORT_TYPES] = {
  T_WALKING,
  T_AIRPORTBUS,
  T_BUS,
  T_DUMMY_VALUE,
  T_AIRPORTTRAIN,
  T_BOAT,
  T_TRAIN,
  T_TRAM,
  T_METRO
};

const realtime_transport_type_t travel_to_realtime[NUM_TRAVEL_TRANSPORT_TYPES] = {
  -1,
  -1,
  R_BUS,
  -1,
  -1,
  R_FERRY,
  R_TRAIN,
  R_TRAM,
  R_METRO
};

const travel_transport_type_t realtime_to_travel[NUM_REALTIME_TRANSPORT_TYPES] = {
  T_BUS,
  T_BOAT,
  T_TRAIN,
  T_TRAM,
  T_METRO
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
