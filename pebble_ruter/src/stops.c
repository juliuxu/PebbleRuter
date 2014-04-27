#include <pebble.h>

#include "libs/pebble-assist.h"

#include "pebble_ruter.h"
#include "stops.h"
#include "util.h"

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
	char *text = malloc(length);
	strcpy(text, tuple->value->cstring);

	APP_LOG(APP_LOG_LEVEL_DEBUG, "handle_put_stops: %s", text);

	int num_strings;
	char **strings = splittoarray(text, length, '~', &num_strings);

	num_ruter_stops = satoi(strings[0]);

	int i;
	int strings_index = 1;
	for (i=0;i<num_ruter_stops;i++) {
		APP_LOG(APP_LOG_LEVEL_DEBUG, "stop: %s %s", strings[strings_index++], strings[strings_index++]);
	}

	free(strings);
	free(text);

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
	uint8_t i;
	for (i=0;i<num_ruter_stops;i++) {
		free(ruter_stops[i].name);
		free(ruter_stops[i].id);
	}
	num_ruter_stops = 0;
}