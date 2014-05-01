#ifndef DEPARTURES_H
#define DEPARTURES_H

#include "pebble_ruter.h"

#define MAX_DEPARTURES 10

struct LineDestination {

	char *line;
	char *destination;

	// Let the JS on the phone compile the departure times string
	// If we later need to dynamicly update departure times without talking to ruter
	// We need to store a list of departure times
	// But for now it's just this:
	// A simple string of departure times, e.g. "now 1 min 4 min 17:32"
	char *departuretimes;

};
typedef struct LineDestination line_destination_t;

uint16_t get_num_departures();
line_destination_t *get_departure(uint8_t index);

void handle_get_departures(char *stopid, realtime_transport_type_t ttype);

void handle_put_departure(DictionaryIterator *iter);
void handle_put_departure_empty(Tuple *tuple);
void handle_put_departure_error(Tuple *tuple);

void destroy_departures(void);
void refresh_departures(void);

#endif