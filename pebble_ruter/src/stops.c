#include <pebble.h>

#include "libs/pebble-assist.h"

#include "pebble_ruter.h"
#include "stops.h"

static uint8_t num_ruter_stops = 0;
static stop_t ruter_stops[MAX_STOPS];

/**
 * Get nearby stops
 */
void get_stops(realtime_transport_type_t ttype) {

	APP_LOG(APP_LOG_LEVEL_DEBUG, "Requesting update of stops of type: %d", ttype);

	DictionaryIterator *iter;
 	app_message_outbox_begin(&iter);

 	dict_write_uint8(iter, GET_STOPS, ttype);

 	app_message_outbox_send();

	return;
}

void handle_put_stops(Tuple *tuple) {
	int length = tuple->length;
	char *text = tuple->value->cstring;

	int part_length = 0;
	char *part_start = text;

	int i;
	for (i=0;i < length;i++) {
		if(part_start+part_length)
	}

}

/**
 * Window callback functions
 */
uint16_t get_num_stops() {
	return num_ruter_stops;
}

stop_t *get_stop(uint8_t index) {
	if (index >= num_ruter_stops) {
		return NULL;
	}
	return &ruter_stops[index];
}

void destroy_stops(void) {
	// int i;
	// for (i=0;i<num_ruter_stops;i++) {
	// 	ruter_stops[i];
	// }
	num_ruter_stops = 0;
}