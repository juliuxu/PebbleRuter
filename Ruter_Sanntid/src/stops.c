#include <pebble.h>

#include "libs/pebble-assist.h"

#include "language.h"

#include "windows/win-stops.h"

#include "pebble_ruter.h"
#include "stops.h"
#include "util.h"

static uint8_t num_ruter_stops = 0;
static stop_t ruter_stops[MAX_STOPS];
static realtime_transport_type_t current_transport_type;

/**
 * Get nearby stops
 */
void handle_get_stops(realtime_transport_type_t ttype) {

	APP_LOG(APP_LOG_LEVEL_DEBUG, "Requesting update of stops of type: %d", ttype);

	DictionaryIterator *iter;
 	app_message_outbox_begin(&iter);

 	if (iter == NULL) {
 		APP_LOG(APP_LOG_LEVEL_DEBUG, "iter is null");
 		return;
 	}

 	dict_write_uint8(iter, GET_STOPS, ttype);

 	app_message_outbox_send();

 	current_transport_type = ttype;

	return;
}

void handle_put_stops(Tuple *tuple) {

	destroy_stops();

	int length = tuple->length;
	char *text = malloc(length);
	strcpy(text, tuple->value->cstring);

	APP_LOG(APP_LOG_LEVEL_DEBUG, "handle_put_stops: %s", text);

	int num_strings;
	char **strings = splittoarray(text, length, '~', &num_strings);

	num_ruter_stops = satoi(strings[0]);

	int i;
	int strings_index;
	for (i=0, strings_index=1;i<num_ruter_stops && i<MAX_STOPS;i++, strings_index+=2) {
		APP_LOG(APP_LOG_LEVEL_DEBUG, "Add Stop: %s %s", strings[strings_index], strings[strings_index+1]);

		ruter_stops[i].id = malloc(strlen(strings[strings_index]));
		ruter_stops[i].name = malloc(strlen(strings[strings_index+1]));

		strcpy(ruter_stops[i].id, strings[strings_index]);
		strcpy(ruter_stops[i].name, strings[strings_index+1]);

	}

	free(strings);
	free(text);

	// Call window update
	refresh_stops_window(current_transport_type);

}


void handle_put_stops_location_success(Tuple *tuple) {
	update_stops_loading_text(tuple->value->uint8, get_language_string(6));
}
void handle_put_stops_location_error(Tuple *tuple) {
	update_stops_loading_text(tuple->value->uint8, get_language_string(7));
}


void handle_put_stops_empty(Tuple *tuple) {
	update_stops_loading_text(tuple->value->uint8, get_language_string(8));
}
void handle_put_stops_error(Tuple *tuple) {
	update_stops_loading_text(tuple->value->uint8, get_language_string(9));
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
	APP_LOG(APP_LOG_LEVEL_DEBUG, "Destroy stops: %d", num_ruter_stops);
	uint8_t i;
	for (i=0;i<num_ruter_stops;i++) {
		if (ruter_stops[i].name == NULL) {
			continue;
		}

		free(ruter_stops[i].name);
		free(ruter_stops[i].id);
	}
	num_ruter_stops = 0;
}