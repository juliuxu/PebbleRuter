#ifndef PEBBLE_RUTER_H
#define PEBBLE_RUTER_H

#include <pebble.h>

#define NUM_REALTIME_TRANSPORT_TYPES 5
#define NUM_TRAVEL_TRANSPORT_TYPES 9

/**
 * The different realtime transport types
 */
typedef enum {
	R_BUS = 0,
	R_FERRY = 1,
	R_RAIL = 2,
	R_TRAIN = R_RAIL, // Alias, because the name train is more understandable..
	R_TRAM = 3,
	R_METRO = 4,
} realtime_transport_type_t;

/**
 * The diffrent travel transport types
 */
typedef enum {
	T_WALKING = 0,
	T_AIRPORTBUS = 1,
	T_BUS = 2,
	T_DUMMY_VALUE = 3,
	T_AIRPORTTRAIN = 4,
	T_BOAT = 5,
	T_TRAIN = 6,
	T_TRAM = 7,
	T_METRO = 8,
} travel_transport_type_t;

/**
 * Global const arrays of transport types, so they can be referenced by memory
 */
extern const realtime_transport_type_t realtime_transport_types[NUM_REALTIME_TRANSPORT_TYPES];
extern const travel_transport_type_t travel_transport_types[NUM_TRAVEL_TRANSPORT_TYPES];

/**
 * Constant maps between realtime and travel id's so we can easily convert back and forth
 */
extern const realtime_transport_type_t travel_to_realtime[NUM_TRAVEL_TRANSPORT_TYPES];
extern const travel_transport_type_t realtime_to_travel[NUM_REALTIME_TRANSPORT_TYPES];

#endif