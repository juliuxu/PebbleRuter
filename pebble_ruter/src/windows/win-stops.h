#ifndef WIN_STOPS_H
#define WIN_STOPS_H

#include "../pebble_ruter.h"

void create_stops_window(transport_type ttype);
void destroy_stops_window(transport_type ttype);
void show_stops_window(transport_type ttype, bool animated);

#endif