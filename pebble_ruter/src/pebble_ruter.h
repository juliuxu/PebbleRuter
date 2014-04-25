#ifndef PEBBLE_RUTER_H
#define PEBBLE_RUTER_H

#include <pebble.h>

#define NUM_REALTIME_TRANSPORT_TYPES 5
#define NUM_TRAVEL_TRANSPORT_TYPES 9

/**
 * The different realtime transport types
 */
typedef enum {

	BUS = 0,
	FERRY = 1,
	RAIL = 2,
	TRAIN = RAIL, // Alias, because the name train is more understandable..
	TRAM = 3,
	METRO = 4,

} realtime_transport_type_t;

/**
 * The diffrent travel transport types
 */
typedef enum {
	WALKING = 0,
	AIRPORTBUS = 1,
	BUS = 2,
	DUMMY_VALUE = 3,
	AIRPORTTRAIN = 4,
	BOAT = 5,
	TRAIN = 6,
	TRAM = 7,
	METRO = 8,
} travel_transport_type_t;

/**
 * Global array of transport types, so they can be referenced by memory
 */
extern const realtime_transport_type_t transport_types[NUM_REALTIME_TRANSPORT_TYPES];
extern const travel_transport_type_t transport_types[NUM_TRAVEL_TRANSPORT_TYPES];


// Map
// Map

#endif