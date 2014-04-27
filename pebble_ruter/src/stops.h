#ifndef STOPS_H
#define STOPS_H

#include "pebble_ruter.h"

#define MAX_STOPS 10

struct Stop {

	char *name;
	char *id;

	// For now just name and id is sufficent

};
typedef struct Stop stop_t;

uint16_t get_num_stops();
stop_t *get_stop(uint8_t index);

void get_stops(realtime_transport_type_t ttype);
void handle_put_stops(Tuple *tuple);

#endif