#include <pebble.h>
#include "../libs/pebble-assist.h"
#include "../libs/bitmap-loader.h"

#include "../pebble_ruter.h"

#include "win-stops.h"

static Window *window;
static MenuLayer *menu_layer;

#define MENU_SECTIONS 1

#define MENU_SECTION_MAIN 0
#define MENU_HEADER_HEIGHT 0
#define MENU_ROWS_MAIN NUM_REALTIME_TRANSPORT_TYPES
#define MENU_CELL_HEIGHT 44

#define MENU_ROW_MAIN_METRO 0
#define MENU_ROW_MAIN_TRAM 1
#define MENU_ROW_MAIN_BUS 2
#define MENU_ROW_MAIN_TRAIN 3
#define MENU_ROW_MAIN_FERRY 4

/** 
 * Callback functions
 */
static uint16_t menu_get_num_sections_callback(MenuLayer *menu_layer, void* callback_context) {
  return MENU_SECTIONS;
}

static uint16_t menu_get_num_rows_callback(MenuLayer *menu_layer, uint16_t section_index, void* callback_context) {
  switch (section_index) {

    case MENU_SECTION_MAIN:
      return MENU_ROWS_MAIN;

    default:
      return 0;
  }
}

static int16_t menu_get_header_height_callback(MenuLayer *menu_layer, uint16_t section_index, void* callback_context) {
  switch (section_index) {
    case MENU_SECTION_MAIN:
      return MENU_HEADER_HEIGHT;
  }
  return 0;
}

static int16_t menu_get_cell_height_callback(MenuLayer *menu_layer, MenuIndex* cell_index, void* callback_context) {
  return MENU_CELL_HEIGHT;
}

static void menu_draw_header_callback(GContext* ctx, const Layer* cell_layer, uint16_t section_index, void* callback_context) {
  switch (section_index) {
    case MENU_SECTION_MAIN:
      // menu_cell_basic_header_draw(ctx, cell_layer, "Hello, Header!");
    break;
  }
}

static void menu_draw_row_callback(GContext* ctx, const Layer *cell_layer, MenuIndex *cell_index, void* callback_context) {
  // Determine which section we're going to draw in
  switch (cell_index->section) {
    case MENU_SECTION_MAIN:
      // Use the row to specify which item we'll draw
      switch (cell_index->row) {
        case MENU_ROW_MAIN_METRO:
          menu_cell_basic_draw(ctx, cell_layer, "Metro", NULL, bitmaps_get_bitmap(RESOURCE_ID_MENU_METRO));
          break;

        case MENU_ROW_MAIN_TRAM:
          menu_cell_basic_draw(ctx, cell_layer, "Tram", NULL, bitmaps_get_bitmap(RESOURCE_ID_MENU_TRAM));
          break;

        case MENU_ROW_MAIN_BUS:
              menu_cell_basic_draw(ctx, cell_layer, "Bus", NULL, bitmaps_get_bitmap(RESOURCE_ID_MENU_BUS));
              break;

        case MENU_ROW_MAIN_TRAIN:
              menu_cell_basic_draw(ctx, cell_layer, "Train", NULL, bitmaps_get_bitmap(RESOURCE_ID_MENU_TRAIN));
              break;

        case MENU_ROW_MAIN_FERRY:
              menu_cell_basic_draw(ctx, cell_layer, "Ferry", NULL, bitmaps_get_bitmap(RESOURCE_ID_MENU_FERRY));
              break;
        

      }
      break;
  }
}

static void menu_select_click_callback(MenuLayer* menu_layer, MenuIndex* cell_index, void* callback_context) {

  switch (cell_index->section) {
    case MENU_SECTION_MAIN:

      switch (cell_index->row) {
        case MENU_ROW_MAIN_METRO:
          show_stops_window(R_METRO, true);        
          break;

        case MENU_ROW_MAIN_TRAM:
          show_stops_window(R_TRAM, true); 
          break;

        case MENU_ROW_MAIN_BUS:
          show_stops_window(R_BUS, true); 
          break;

        case MENU_ROW_MAIN_TRAIN:
          show_stops_window(R_TRAIN, true); 
          break;

        case MENU_ROW_MAIN_FERRY:
          show_stops_window(R_FERRY, true); 
          break;
      }
    break;

  }
}


static void window_load(Window* window) {
  menu_layer = menu_layer_create_fullscreen(window);
  menu_layer_set_callbacks(menu_layer, NULL, (MenuLayerCallbacks){
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
}

static void window_unload(Window* window) {
  menu_layer_destroy(menu_layer);
}

void create_main_window(void) {
  // Create the stops windows
  int i;
  for (i=0;i<NUM_REALTIME_TRANSPORT_TYPES;i++) {
    create_stops_window(i);
  }
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Created all stops");
  window = window_create();
  window_set_window_handlers(window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
  });
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Created main window");
}

void destroy_main_window(void) {

  APP_LOG(APP_LOG_LEVEL_DEBUG, "Destorying stops");
  // Destroy the stops windows
  int i;
  for (i=0;i<NUM_REALTIME_TRANSPORT_TYPES;i++) {
    destroy_stops_window(i);
  }
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Destroyed all stops");
  window_destroy(window);
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Destroyed main window");
}

void show_main_window(bool animated) {
  window_stack_push(window, animated);
}