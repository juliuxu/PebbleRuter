#include <pebble.h>
#include "util.h"
#include "pebble_ruter.h"
#include "stops.h"
#include "departures.h"
#include "windows/win-main.h"

#include "libs/bitmap-loader.h"
#include "libs/message-handler.h"

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

static void in_received_handler(DictionaryIterator *received, void *context) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Received a message!");

  Tuple *tuple;


  /**
   * Put stops
   */
  if ( (tuple = dict_find(received, PUT_STOPS)) ) {
   APP_LOG(APP_LOG_LEVEL_DEBUG, "PUT_STOPS");
   handle_put_stops(tuple);
  }

  /**
   * Location status
   */
  else if ( (tuple = dict_find(received, PUT_STOPS_LOCATION_SUCCESS)) ) {
   APP_LOG(APP_LOG_LEVEL_DEBUG, "PUT_STOPS_LOCATION_SUCCESS");
   handle_put_stops_location_success(tuple);
  }
  else if ( (tuple = dict_find(received, PUT_STOPS_LOCATION_ERROR)) ) {
   APP_LOG(APP_LOG_LEVEL_DEBUG, "PUT_STOPS_LOCATION_ERROR");
   handle_put_stops_location_error(tuple);
  }

  /**
   * Put stops status
   */
  else if ( (tuple = dict_find(received, PUT_STOPS_EMPTY)) ) {
   APP_LOG(APP_LOG_LEVEL_DEBUG, "PUT_STOPS_EMPTY");
   handle_put_stops_empty(tuple);
  }
  else if ( (tuple = dict_find(received, PUT_STOPS_ERROR)) ) {
   APP_LOG(APP_LOG_LEVEL_DEBUG, "PUT_STOPS_ERROR");
   handle_put_stops_error(tuple);
  }


  /**
   * Put Departure
   */
  else if ( (tuple = dict_find(received, PUT_DEPARTURE)) ) {
   APP_LOG(APP_LOG_LEVEL_DEBUG, "PUT_DEPARTURE");
   handle_put_departure(received);
  }

  /**
   * Put departure status
   */
  else if ( (tuple = dict_find(received, PUT_DEPARTURE_EMPTY)) ) {
   APP_LOG(APP_LOG_LEVEL_DEBUG, "PUT_DEPARTURE_EMPTY");
   handle_put_departure_empty(tuple);
  }
  else if ( (tuple = dict_find(received, PUT_DEPARTURE_ERROR)) ) {
   APP_LOG(APP_LOG_LEVEL_DEBUG, "PUT_DEPARTURE_ERROR");
   handle_put_departure_error(tuple);
  }

  /**
   * Invalid message
   */
  else {
   APP_LOG(APP_LOG_LEVEL_DEBUG, "Invalid message!");
  }

}

static void in_dropped_handler(AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "App Message In Dropped!: %s", translate_error(reason));
}


/**
 * Init
 */
static void app_message_init(void) {
  // Register message handlers
  app_message_register_inbox_received(in_received_handler);
  app_message_register_inbox_dropped(in_dropped_handler);
  app_message_register_outbox_failed(message_handler_outbox_failed_handler);
  app_message_register_outbox_sent(message_handler_outbox_sent_handler);


  APP_LOG(APP_LOG_LEVEL_DEBUG, "max inbox: %d max outbox: %d", (int) app_message_inbox_size_maximum(), (int) app_message_outbox_size_maximum());
  // Init buffers
  app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());

}

static void init(void) {
  app_message_init();

  bitmaps_init();

  create_main_window();
  show_main_window(true);
}

static void deinit(void) {

  bitmaps_cleanup();

  destroy_messages();

  destroy_departures();
  destroy_stops();
  destroy_main_window();
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}
