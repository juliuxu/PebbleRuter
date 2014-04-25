#ifndef STOPS_H
#define STOPS_H

#include "pebble_ruter.h"

#define MAX_STOPS 10

struct Stop {

	char name[STRING_DATA_SIZE];
	char id[STRING_DATA_SIZE];

	// For now just name and id is sufficent

};
typedef struct Stop stop_t;

void update_stops(realtime_transport_type_t ttype);

#endif