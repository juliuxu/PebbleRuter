#ifndef DEPARTURES_H
#define DEPARTURES_H

#include "pebble_ruter.h"

struct LineDestination {

	char *line_destination;

	// Let the JS on the phone compile the depature times string
	// If we later need to dynamicly update depature times without talking to ruter
	// We need to store a list of depature times
	// But for now it's just this:
	// A simple string of depature times, e.g. "now 1 min 4 min 17:32"
	char *depaturetimes;

};
typedef struct LineDestination line_destination_t;

uint16_t get_num_depatures();
stop_t *get_depature(uint8_t index);

void get_depatures(realtime_transport_type_t ttype);
void handle_put_depatures(Tuple *tuple);
void destroy_depatures(void);
void refresh_depatures_window(realtime_transport_type_t ttype);

#endif