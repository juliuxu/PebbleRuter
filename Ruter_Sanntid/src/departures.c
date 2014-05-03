#include <pebble.h>

#include "libs/pebble-assist.h"
#include "libs/message-handler.h"

#include "language.h"

#include "windows/win-departures.h"

#include "pebble_ruter.h"
#include "departures.h"
#include "util.h"

static uint8_t num_ruter_departures = 0;
static line_destination_t ruter_departures[MAX_DEPARTURES];

static realtime_transport_type_t current_transport_type;
static char *current_stopid;

/**
 * Handler for when message could not get sent
 */
void handle_get_departures_failure(void) {
	update_departures_loading_text(current_transport_type, get_language_string(14));
}

/**
 * Get departures
 */
void handle_get_departures(char* stopid, realtime_transport_type_t ttype) {
	APP_LOG(APP_LOG_LEVEL_DEBUG, "Requesting update of departures of type: %d", ttype);

 	current_transport_type = ttype;
 	current_stopid = stopid;

	dict_entry_t **dicts = dict_entries_create(2);

	dicts[0]->key = GET_DEPARTURES_STOPID;
	dicts[0]->type = CSTRING;
	dicts[0]->value.cstring = strdup(stopid);

	dicts[1]->key = GET_DEPARTURES;
	dicts[1]->type = UINT8;
	dicts[1]->value.uint8 = ttype;

	send_message(dicts, 2, NULL, handle_get_departures_failure);

	return;
}

/**
 * Refresh departures
 */
void refresh_departures(void) {
	handle_get_departures(current_stopid, current_transport_type);
}

void handle_put_departure(DictionaryIterator *iter) {
	Tuple* t_departure = dict_find(iter, PUT_DEPARTURE);
	Tuple* t_departure_index = dict_find(iter, PUT_DEPARTURE_INDEX);
	Tuple* t_departure_length = dict_find(iter, PUT_DEPARTURE_LENGTH);

	uint8_t index = t_departure_index->value->uint8;
	uint8_t length = t_departure_length->value->uint8;


	// Check if this is the first of more to come
	if (index == 0) {
		// If so we can destroy our departures
		destroy_departures();
	}
	else if (index >= MAX_DEPARTURES) {
		// To high cannot accept
		APP_LOG(APP_LOG_LEVEL_DEBUG, "To high index %d", index);
		return;
	}
	else if (ruter_departures[index].line != NULL) {
		// We are overwriting an existing entry
		// Probably a duplicate, so we can just return
		return;
	}

	int text_length = t_departure->length;
	char *text = malloc(text_length);
	strcpy(text, t_departure->value->cstring);

	int num_strings;
	char **strings = splittoarray(text, text_length, '~', &num_strings);
	if (num_strings != 3) {
		free(strings);
		free(text);
		return;
	}

	APP_LOG(APP_LOG_LEVEL_DEBUG, "Add line_destination_t: %s %s %s", strings[0], strings[1], strings[2]);

	ruter_departures[index].line = malloc(strlen(strings[0]));
	ruter_departures[index].destination = malloc(strlen(strings[1]));
	ruter_departures[index].departuretimes = malloc(strlen(strings[2]));

	strcpy(ruter_departures[index].line, strings[0]);
	strcpy(ruter_departures[index].destination, strings[1]);
	strcpy(ruter_departures[index].departuretimes, strings[2]);

	free(strings);
	free(text);

	// Check if this was the last departure
	// Or if we cannot accept any more
	if (index + 1 == length ||
		index + 1 == MAX_DEPARTURES
		) {

		// See if the timestamp is set
		Tuple* t_departure_time = dict_find(iter, PUT_DEPARTURE_TIME);
		if (t_departure_time) {
			// Update the last refresh time
			APP_LOG(APP_LOG_LEVEL_DEBUG, "Last Refresh Time: %s", t_departure_time->value->cstring);
			update_last_refresh_time(t_departure_time->value->cstring);
		}
		else {
			// Set the last update time to n/a
			update_last_refresh_time("n/a");
		}

		// Time to update window
		num_ruter_departures = length;
		refresh_departures_window(current_transport_type);
	}
	else {
		num_ruter_departures = index + 1;
	}

}

void handle_put_departure_empty(Tuple *tuple) {
	update_departures_loading_text(tuple->value->uint8, get_language_string(12));
	destroy_departures();
	reload_menu_data(current_transport_type);
}
void handle_put_departure_error(Tuple *tuple) {
	update_departures_loading_text(tuple->value->uint8, get_language_string(13));
	reload_menu_data(current_transport_type);
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
	APP_LOG(APP_LOG_LEVEL_DEBUG, "Destroy departures %d", num_ruter_departures);
	uint8_t i;
	for (i=0;i<num_ruter_departures;i++) {
		if (ruter_departures[i].line == NULL) {
			continue;
		}

		free(ruter_departures[i].line);
		free(ruter_departures[i].destination);
		free(ruter_departures[i].departuretimes);

		/* Null out */
		ruter_departures[i].line = NULL;
		ruter_departures[i].destination = NULL;
		ruter_departures[i].departuretimes = NULL;
	}
	num_ruter_departures = 0;
}