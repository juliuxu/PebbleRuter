#include <pebble.h>

#include "libs/pebble-assist.h"

#include "pebble_ruter.h"
#include "depatures.h"
#include "util.h"

static uint8_t num_ruter_depatures = 0;
static line_destination_t ruter_depatures[MAX_depatures];
static realtime_transport_type_t current_transport_type;

/**
 * Get depatures
 */
void handle_get_depatures(realtime_transport_type_t ttype) {

	APP_LOG(APP_LOG_LEVEL_DEBUG, "Requesting update of depatures of type: %d", ttype);

	DictionaryIterator *iter;
 	app_message_outbox_begin(&iter);

 	dict_write_uint8(iter, GET_DEPATURES, ttype);

 	app_message_outbox_send();

 	current_transport_type = ttype;

	return;
}

void handle_put_depatures(Tuple *tuple) {

	destroy_depatures();

	int length = tuple->length;
	char *text = malloc(length);
	strcpy(text, tuple->value->cstring);

	APP_LOG(APP_LOG_LEVEL_DEBUG, "handle_put_depatures: %s", text);

	int num_strings;
	char **strings = splittoarray(text, length, '~', &num_strings);

	num_ruter_depatures = satoi(strings[0]);

	int i;
	int strings_index;
	for (i=0, strings_index=1;i<num_ruter_depatures && i<MAX_depatures;i++, strings_index+=2) {
		APP_LOG(APP_LOG_LEVEL_DEBUG, "Add Stop: %s %s", strings[strings_index], strings[strings_index+1]);

		ruter_depatures[i].id = malloc(strlen(strings[strings_index]));
		ruter_depatures[i].name = malloc(strlen(strings[strings_index+1]));

		strcpy(ruter_depatures[i].id, strings[strings_index]);
		strcpy(ruter_depatures[i].name, strings[strings_index+1]);

	}

	free(strings);
	free(text);

	// Call window update
	refresh_depatures_window(current_transport_type);

}

/**
 * Window callback functions
 */
uint16_t get_num_depatures() {
	return num_ruter_depatures;
}

line_destination_t *get_depature(uint8_t index) {
	if (index >= num_ruter_depatures) {
		return NULL;
	}
	return &ruter_depatures[index];
}

void destroy_depatures(void) {
	uint8_t i;
	for (i=0;i<num_ruter_depatures;i++) {
		free(ruter_depatures[i].name);
		free(ruter_depatures[i].id);
	}
	num_ruter_depatures = 0;
}