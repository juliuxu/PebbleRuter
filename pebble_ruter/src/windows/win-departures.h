#ifndef WIN_DEPARTURES_H
#define WIN_DEPARTURES_H

#include "../pebble_ruter.h"

void create_departures_window(realtime_transport_type_t ttype);
void destroy_departures_window(realtime_transport_type_t ttype);
void show_departures_window(char *stopid, realtime_transport_type_t ttype, bool animated);
void refresh_departures_window(realtime_transport_type_t ttype);

#endif