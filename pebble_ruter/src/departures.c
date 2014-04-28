#include <pebble.h>

#include "libs/pebble-assist.h"

#include "pebble_ruter.h"
#include "departures.h"
#include "util.h"

static uint8_t num_ruter_departures = 0;
static line_destination_t ruter_departures[MAX_DEPARTURES];

static realtime_transport_type_t current_transport_type;
static char *current_stopid;
/**
 * Get departures
 */
void handle_get_departures(char* stopid, realtime_transport_type_t ttype) {

	APP_LOG(APP_LOG_LEVEL_DEBUG, "Requesting update of departures of type: %d", ttype);

	DictionaryIterator *iter;
 	app_message_outbox_begin(&iter);

 	dict_write_cstring(iter, GET_DEPARTURES_STOPID, stopid);
 	dict_write_uint8(iter, GET_DEPARTURES, ttype);

 	app_message_outbox_send();

 	current_transport_type = ttype;
 	current_stopid = stopid;

	return;
}

void handle_put_departures(Tuple *tuple) {

	destroy_departures();

	int length = tuple->length;
	char *text = malloc(length);
	strcpy(text, tuple->value->cstring);

	APP_LOG(APP_LOG_LEVEL_DEBUG, "handle_put_departures(%d): %s", length, text);

	int num_strings;
	char **strings = splittoarray(text, length, '~', &num_strings);

	num_ruter_departures = satoi(strings[0]);
	APP_LOG(APP_LOG_LEVEL_DEBUG, "num_ruter_departures: %d", num_ruter_departures);

	int i;
	int strings_index;
	for (i=0, strings_index=1;i<num_ruter_departures && i<MAX_DEPARTURES;i++, strings_index+=3) {
		APP_LOG(APP_LOG_LEVEL_DEBUG, "Add line_destination_t: %s %s %s", strings[strings_index], strings[strings_index+1], strings[strings_index+2]);

		ruter_departures[i].line = malloc(strlen(strings[strings_index]));
		ruter_departures[i].destination = malloc(strlen(strings[strings_index+1]));
		ruter_departures[i].departuretimes = malloc(strlen(strings[strings_index+2]));

		strcpy(ruter_departures[i].line, strings[strings_index]);
		strcpy(ruter_departures[i].destination, strings[strings_index+1]);
		strcpy(ruter_departures[i].departuretimes, strings[strings_index+2]);

	}

	free(strings);
	free(text);

	// Call window update
	refresh_departures_window(current_transport_type);

}

/**
 * Window callback functions
 */
uint16_t get_num_departures() {
	return num_ruter_departures;
}

line_destination_t *get_departure(uint8_t index) {
	if (index >= num_ruter_departures) {
		return NULL;
	}
	return &ruter_departures[index];
}

void destroy_departures(void) {
	uint8_t i;
	for (i=0;i<num_ruter_departures;i++) {
		free(ruter_departures[i].line);
		free(ruter_departures[i].destination);
		free(ruter_departures[i].departuretimes);
	}
	num_ruter_departures = 0;
}