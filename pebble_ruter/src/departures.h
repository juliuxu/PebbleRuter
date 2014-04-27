#ifndef DEPARTURES_H
#define DEPARTURES_H

#include "pebble_ruter.h"

struct LineDestination {

	char line_destination[STRING_DATA_SIZE];

	// Let the JS on the phone compile the depature times string
	// If we later need to dynamicly update depature times without talking to ruter
	// We need to store a list of depature times
	// But for now it's just this:
	// A simple string of depature times, e.g. "now 1 min 4 min 17:32"
	char depaturetimes[STRING_DATA_SIZE];

};
typedef struct LineDestination line_destination_t;

#endif