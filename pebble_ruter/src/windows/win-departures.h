#ifndef WIN_DEPARTURES_H
#define WIN_DEPARTURES_H

#include "../pebble_ruter.h"

void create_stops_window(transport_type_t ttype);
void destroy_stops_window(transport_type_t ttype);
void show_stops_window(transport_type_t ttype, bool animated);

#endif