#ifndef WIN_STOPS_H
#define WIN_STOPS_H

#include "../pebble_ruter.h"

void create_stops_window(realtime_transport_type_t ttype);
void destroy_stops_window(realtime_transport_type_t ttype);
void show_stops_window(realtime_transport_type_t ttype, bool animated);
void refresh_stops_window(realtime_transport_type_t ttype);

#endif