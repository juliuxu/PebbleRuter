#ifndef PEBBLE_RUTER_H
#define PEBBLE_RUTER_H

#include <pebble.h>

#define NUM_TRANSPORT_TYPES 5

/**
 * The different transport types
 */
typedef enum {

	BUS = 0,
	FERRY = 1,
	RAIL = 2,
	TRAIN = RAIL, // Alias, because the name train is more understandable..
	TRAM = 3,
	METRO = 4,

} transport_type_t;

/**
 * Global array of transport types, so they can be referenced by memory
 */
extern const transport_type_t transport_types[NUM_TRANSPORT_TYPES];

#endif