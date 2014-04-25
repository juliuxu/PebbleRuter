#include <pebble.h>

#include "libs/pebble-assist.h"

#include "pebble_ruter.h"
#include "stops.h"

int ruter_stops_length = 0;
static stop_t ruter_stops[MAX_STOPS];

void get_stops(realtime_transport_type_t ttype) {
	return;
}