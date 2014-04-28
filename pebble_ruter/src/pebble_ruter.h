#ifndef PEBBLE_RUTER_H
#define PEBBLE_RUTER_H

#include <pebble.h>

#define NUM_REALTIME_TRANSPORT_TYPES 5

/**
 * The Different messages we can send and receive via AppMessage
 */
typedef enum {

	// For sending
	GET_STOPS,
	GET_DEPARTURES,
	GET_DEPARTURES_STOPID,

	// For receving
	PUT_STOPS,
	PUT_DEPARTURES,

} appmessage_type_t;

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
 * Global const arrays of transport types, so they can be referenced by memory
 */
extern const realtime_transport_type_t realtime_transport_types[NUM_REALTIME_TRANSPORT_TYPES];

#endif