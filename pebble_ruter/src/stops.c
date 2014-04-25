#include <pebble.h>

#include "libs/pebble-assist.h"

#include "pebble_ruter.h"
#include "stops.h"

int ruter_stops_length = 0;
static stop_t ruter_stops[MAX_STOPS];

/**
 * Update stops
 */
void update_stops(realtime_transport_type_t ttype) {

	APP_LOG(APP_LOG_LEVEL_DEBUG, "Requesting update of stops of type: %d", ttype);

	DictionaryIterator *iter;
 	app_message_outbox_begin(&iter);

 	dict_write_uint8(iter, 32, realtime_to_travel[ttype]);

 	app_message_outbox_send();

	return;
}