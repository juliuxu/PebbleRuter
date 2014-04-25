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

/**
 * AppMessage Handlers
 */

static void out_sent_handler(DictionaryIterator *sent, void *context) {
 // outgoing message was delivered
}


static void out_failed_handler(DictionaryIterator *failed, AppMessageResult reason, void *context) {
 APP_LOG(APP_LOG_LEVEL_DEBUG, "App Message Failed to Send!");
}


static void in_received_handler(DictionaryIterator *received, void *context) {
 APP_LOG(APP_LOG_LEVEL_DEBUG, "Received a message!");

 Tuple *text_tuple;

 if ( (text_tuple = dict_find(received, PUT_STOPS)) ) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "PUT_STOPS");
 }
 else if ( (text_tuple = dict_find(received, PUT_DEPATURES)) ) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "PUT_DEPATURES");

 }
 else {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Invalid message!");
 }

}


static void in_dropped_handler(AppMessageResult reason, void *context) {
 APP_LOG(APP_LOG_LEVEL_DEBUG, "App Message Dropped!");
}

static void app_message_init(void) {
  // Register message handlers
  app_message_register_inbox_received(in_received_handler);
  app_message_register_inbox_dropped(in_dropped_handler);
  app_message_register_outbox_failed(out_failed_handler);
  app_message_register_outbox_sent(out_sent_handler);

  // Init buffers
  app_message_open(64, 64);

}

static void init(void) {
  app_message_init();

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
