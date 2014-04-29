#include <pebble.h>
#include "../libs/pebble-assist.h"

#include "../pebble_ruter.h"
#include "../stops.h"

#include "../layers/layer-loading.h"

#include "win-departures.h"

#define MENU_SECTIONS 1

#define MENU_SECTION_MAIN 0
#define MENU_HEADER_HEIGHT 0
#define MENU_CELL_HEIGHT 44

/**
 * Array of pointers to all the stops windows and layers, indexed by transport type
 */
static Window *transport_type_to_window_map[NUM_REALTIME_TRANSPORT_TYPES];
static MenuLayer *transport_type_to_menulayer_map[NUM_REALTIME_TRANSPORT_TYPES];
static LoadingLayer *transport_type_to_loadinglayer_map[NUM_REALTIME_TRANSPORT_TYPES];

/**
 * Get transport type from window pointer,
 * used by callback functions to know what kind of window they are dealing with
 */
static int8_t get_transport_type_from_window(Window *window) {
  int i;
  for (i=0;i<NUM_REALTIME_TRANSPORT_TYPES;i++) {
    if (transport_type_to_window_map[i] == window) {
      return i;
    }

  }
  return -1;
}

/**
 * Refresh
 */
void refresh_stops_window(realtime_transport_type_t ttype) {
  if (!window_stack_contains_window(transport_type_to_window_map[ttype])) {
    return;
  }

  menu_layer_reload_data(transport_type_to_menulayer_map[ttype]);
  layer_hide(transport_type_to_loadinglayer_map[ttype]);
}

/** 
 * Window Callback functions
 */
static uint16_t menu_get_num_sections_callback(MenuLayer* menu_layer, void *callback_context) {
  return MENU_SECTIONS;
}

static uint16_t menu_get_num_rows_callback(MenuLayer *menu_layer, uint16_t section_index, void *callback_context) {
  switch (section_index) {
    case MENU_SECTION_MAIN:

      return get_num_stops();

    default:
      return 0;
  }
}

static int16_t menu_get_header_height_callback(MenuLayer* menu_layer, uint16_t section_index, void *callback_context) {
  switch (section_index) {
    case MENU_SECTION_MAIN:
      return MENU_HEADER_HEIGHT;
  }
  return 0;
}

static int16_t menu_get_cell_height_callback(MenuLayer* menu_layer, MenuIndex* cell_index, void *callback_context) {
  return MENU_CELL_HEIGHT;
}

static void menu_draw_header_callback(GContext* ctx, const Layer* cell_layer, uint16_t section_index, void *callback_context) {
  switch (section_index) {
    case MENU_SECTION_MAIN:
      // menu_cell_basic_header_draw(ctx, cell_layer, "Hello, Header!");
    break;
  }
}

static void menu_draw_row_callback(GContext* ctx, const Layer *cell_layer, MenuIndex *cell_index, void *callback_context) {
  
  stop_t *stop;
  switch (cell_index->section) {
    case MENU_SECTION_MAIN:
      stop = get_stop(cell_index->row);
      if (stop == NULL) {
        return;
      }

      menu_cell_basic_draw(ctx, cell_layer, stop->name, NULL, NULL);
      break;
  }
}

static void menu_select_click_callback(MenuLayer* menu_layer, MenuIndex* cell_index, void* callback_context) {

  realtime_transport_type_t *ttype = (realtime_transport_type_t *) callback_context;

  stop_t *stop;
  switch (cell_index->section) {
    case MENU_SECTION_MAIN:
      stop = get_stop(cell_index->row);
      if (stop == NULL) {
        return;
      }

      show_departures_window(stop->id, *ttype, true);
      break;

  }
}


static void window_load(Window *window) {

  realtime_transport_type_t ttype = get_transport_type_from_window(window);

  MenuLayer *menu_layer = menu_layer_create_fullscreen(window);
  menu_layer_set_callbacks(menu_layer, (void *) &realtime_transport_types[ttype], (MenuLayerCallbacks){
    .get_num_sections = menu_get_num_sections_callback,
    .get_num_rows = menu_get_num_rows_callback,
    .get_header_height = menu_get_header_height_callback,
    .get_cell_height = menu_get_cell_height_callback,
    .draw_header = menu_draw_header_callback,
    .draw_row = menu_draw_row_callback,
    .select_click = menu_select_click_callback
  });
  menu_layer_set_click_config_onto_window(menu_layer, window);
  menu_layer_add_to_window(menu_layer, window);

  // Set menu layer
  transport_type_to_menulayer_map[ttype] = menu_layer;

  // Set loading layer
  LoadingLayer *loading_layer = loading_layer_create(window);
  loading_layer_set_text(loading_layer, "Getting Location and Loading stops");
  transport_type_to_loadinglayer_map[ttype] = loading_layer;
}

static void window_unload(Window *window) {
  realtime_transport_type_t ttype = get_transport_type_from_window(window);
  menu_layer_destroy(transport_type_to_menulayer_map[ttype]);
  loading_layer_destroy(transport_type_to_loadinglayer_map[ttype]);
}

void create_stops_window(realtime_transport_type_t ttype) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Create stops windows %d", ttype);

  create_departures_window(ttype);

  Window *window = window_create();
  window_set_window_handlers(window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
  });

  transport_type_to_window_map[ttype] = window;

}

void destroy_stops_window(realtime_transport_type_t ttype) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Destroy stops windows %d", ttype);

  destroy_departures_window(ttype);

  window_destroy(transport_type_to_window_map[ttype]);
}

void show_stops_window(realtime_transport_type_t ttype, bool animated) {
  handle_get_stops(ttype);
  window_stack_push(transport_type_to_window_map[ttype], animated);
  layer_show(transport_type_to_loadinglayer_map[ttype]);
}