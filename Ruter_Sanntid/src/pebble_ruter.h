#ifndef PEBBLE_RUTER_H
#define PEBBLE_RUTER_H

#include <pebble.h>

#define NUM_REALTIME_TRANSPORT_TYPES 5

/**
 * The Different messages we can send and receive via AppMessage
 */
typedef enum {

    GET_STOPS = 0,
    GET_DEPARTURES = 1,
    GET_DEPARTURES_STOPID = 2,

    PUT_STOPS = 3,

    PUT_STOPS_LOCATION_SUCCESS = 4,
    PUT_STOPS_LOCATION_ERROR = 5,

    PUT_STOPS_EMPTY = 6,
    PUT_STOPS_ERROR = 7,

    PUT_DEPARTURE = 8,
    PUT_DEPARTURE_INDEX = 9,
    PUT_DEPARTURE_LENGTH = 10,
    PUT_DEPARTURE_TIME = 11,

    PUT_DEPARTURE_EMPTY = 12,
    PUT_DEPARTURE_ERROR = 13

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