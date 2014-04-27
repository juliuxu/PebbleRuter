#include <pebble.h>
#include "pebble_ruter.h"
#include "stops.h"
#include "departures.h"
#include "windows/win-main.h"

/**
 * Init transport enum info
 */
const realtime_transport_type_t realtime_transport_types[NUM_REALTIME_TRANSPORT_TYPES] = {
  R_BUS,
  R_FERRY,
  R_TRAIN,
  R_TRAM,
  R_METRO,
};

/**
 * AppMessage Handlers
 */
static void out_sent_handler(DictionaryIterator *sent, void *context) {
 APP_LOG(APP_LOG_LEVEL_DEBUG, "App Message Sent!");
}


static void out_failed_handler(DictionaryIterator *failed, AppMessageResult reason, void *context) {
 APP_LOG(APP_LOG_LEVEL_DEBUG, "App Message Failed to Send!");
}


static void in_received_handler(DictionaryIterator *received, void *context) {
 APP_LOG(APP_LOG_LEVEL_DEBUG, "Received a message!");

 Tuple *tuple;

 if ( (tuple = dict_find(received, PUT_STOPS)) ) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "PUT_STOPS");
  handle_put_stops(tuple);
 }
 else if ( (tuple = dict_find(received, PUT_DEPATURES)) ) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "PUT_DEPATURES");

 }
 else {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Invalid message!");
 }

}


static void in_dropped_handler(AppMessageResult reason, void *context) {
 APP_LOG(APP_LOG_LEVEL_DEBUG, "App Message Dropped!");
}


/**
 * Init
 */
static void app_message_init(void) {
  // Register message handlers
  app_message_register_inbox_received(in_received_handler);
  app_message_register_inbox_dropped(in_dropped_handler);
  app_message_register_outbox_failed(out_failed_handler);
  app_message_register_outbox_sent(out_sent_handler);

  // Init buffers
  app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());

}

static void init(void) {
  app_message_init();

  create_main_window();
  show_main_window(true);
}

static void deinit(void) {
  destroy_stops();
  destroy_main_window();
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}
