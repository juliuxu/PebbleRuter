#include <pebble.h>
#include "../libs/pebble-assist.h"

#include "../pebble_ruter.h"

#include "win-departures.h"

#define MENU_SECTIONS 1

#define MENU_SECTION_MAIN 0
#define MENU_HEADER_HEIGHT 0
#define MENU_CELL_HEIGHT 35

/**
 * Array of pointers to all the stops windows, indexed by transport type
 */
static Window *transport_type_to_window_map[NUM_TRANSPORT_TYPES];
static MenuLayer *transport_type_to_menulayer_map[NUM_TRANSPORT_TYPES];

/**
 * Get transport type from window pointer,
 * used by callback functions to know what kind of window they are dealing with
 */
static int8_t get_transport_type_from_window(Window *window) {
  int i;
  for (i=0;i<NUM_TRANSPORT_TYPES;i++) {
    if (transport_type_to_window_map[i] == window) {
      return i;
    }

  }
  return -1;
}

/** 
 * Callback functions
 */
static uint16_t menu_get_num_sections_callback(MenuLayer* menu_layer, void *callback_context) {
  return MENU_SECTIONS;
}

static uint16_t menu_get_num_rows_callback(MenuLayer *menu_layer, uint16_t section_index, void *callback_context) {
  switch (section_index) {
    case MENU_SECTION_MAIN:

      // TODO
      return 6;

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
  
  transport_type_t *ttype = (transport_type_t *) callback_context;

  switch (cell_index->section) {
    case MENU_SECTION_MAIN:
      
      switch (*ttype) {
        case BUS:
          menu_cell_basic_draw(ctx, cell_layer, "Bislett buss", "bislett buss", NULL);
          break;

        case FERRY:
          menu_cell_basic_draw(ctx, cell_layer, "Aker Brygge", "baat", NULL);
          break;

        case TRAIN:
          menu_cell_basic_draw(ctx, cell_layer, "Nationaltheatret", "nationaltheatret tog", NULL);
          break;

        case TRAM:
          menu_cell_basic_draw(ctx, cell_layer, "Bislett trikk", "bislett trikk", NULL);
          break;

        case METRO:
          menu_cell_basic_draw(ctx, cell_layer, "Bislett tbane", "bislett tbane", NULL);
          break;


        break;
      }

      break;
  }
}

static void menu_select_click_callback(MenuLayer* menu_layer, MenuIndex* cell_index, void* callback_context) {

  transport_type_t *ttype = (transport_type_t *) callback_context;

  switch (cell_index->section) {
    case MENU_SECTION_MAIN:
      // TODO
    break;

  }
}


static void window_load(Window *window) {

  int ttype = get_transport_type_from_window(window);

  MenuLayer *menu_layer = menu_layer_create_fullscreen(window);
  menu_layer_set_callbacks(menu_layer, (void *) &transport_types[ttype], (MenuLayerCallbacks){
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
}

static void window_unload(Window *window) {
  menu_layer_destroy(transport_type_to_menulayer_map[get_transport_type_from_window(window)]);
}

void create_stops_window(transport_type_t ttype) {

  Window *window = window_create();
  window_set_window_handlers(window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
  });

  transport_type_to_window_map[ttype] = window;

}

void destroy_stops_window(transport_type_t ttype) {
  window_destroy(transport_type_to_window_map[ttype]);
  transport_type_to_window_map[ttype] = NULL;
}

void show_stops_window(transport_type_t ttype, bool animated) {
  window_stack_push(transport_type_to_window_map[ttype], animated);
}