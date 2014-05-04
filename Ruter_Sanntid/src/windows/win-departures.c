#include <pebble.h>
#include "../libs/pebble-assist.h"

#include "../language.h"

#include "../layers/layer-loading.h"

#include "../pebble_ruter.h"
#include "../departures.h"

#define MENU_SECTIONS 1

#define MENU_SECTION_MAIN 0
#define MENU_HEADER_HEIGHT 16
#define MENU_CELL_HEIGHT 44

/**
 * Array of pointers to all the departure windows and layers, indexed by transport type
 */
static Window *transport_type_to_window_map[NUM_REALTIME_TRANSPORT_TYPES];
static MenuLayer *transport_type_to_menulayer_map[NUM_REALTIME_TRANSPORT_TYPES];
static LoadingLayer *transport_type_to_loadinglayer_map[NUM_REALTIME_TRANSPORT_TYPES];

/**
 * The last time the departures where updated
 */
static char last_refresh_time[9] = "n/a";

/**
 * Refresh every 30 sec
 */
#define REFRESH_TIMEOUT 30000
static AppTimer *refresh_timer;

/**
 * Timeout after sending a command to the phone
 */
#define COMMAND_TIMEOUT 10000
static AppTimer *command_timeout_timer = NULL;

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
 * Reload Menu Data
 */
void reload_menu_data(realtime_transport_type_t ttype) {
  if (!window_stack_contains_window(transport_type_to_window_map[ttype])) {
    return;
  }

  menu_layer_reload_data(transport_type_to_menulayer_map[ttype]);
}

/**
 * Refresh Window
 */
void refresh_departures_window(realtime_transport_type_t ttype) {
  if (!window_stack_contains_window(transport_type_to_window_map[ttype])) {
    return;
  }

  menu_layer_reload_data(transport_type_to_menulayer_map[ttype]);
  layer_hide(transport_type_to_loadinglayer_map[ttype]);

  if (command_timeout_timer != NULL) {
    app_timer_cancel(command_timeout_timer);
    command_timeout_timer = NULL;
  }
}

/**
 * Update loading layer text
 */

void update_departures_loading_text(realtime_transport_type_t ttype, char *text) {
  if (!window_stack_contains_window(transport_type_to_window_map[ttype])) {
    return;
  }

  loading_layer_set_text(transport_type_to_loadinglayer_map[ttype], text);

  if (command_timeout_timer != NULL) {
    app_timer_cancel(command_timeout_timer);
    command_timeout_timer = NULL;
  }
}

/**
 * Update last refresh time text
 */
void update_last_refresh_time(char *text) {
  strncpy(last_refresh_time, text, sizeof(last_refresh_time));
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
      return get_num_departures();
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

  char refresh_text[24];

  switch (section_index) {
    case MENU_SECTION_MAIN:
      snprintf(refresh_text, sizeof(refresh_text), get_language_string(10), last_refresh_time);
      menu_cell_basic_header_draw(ctx, cell_layer, refresh_text);
    break;
  }
}

static void menu_draw_row_callback(GContext* ctx, const Layer *cell_layer, MenuIndex *cell_index, void *callback_context) {
  
  //realtime_transport_type_t *ttype = (realtime_transport_type_t *) callback_context;
  line_destination_t *linedest;

  char departure_title[48];

  switch (cell_index->section) {
    case MENU_SECTION_MAIN:
      linedest = get_departure(cell_index->row);
      if (linedest == NULL) {
        return;
      }

      snprintf(departure_title, sizeof(departure_title), "%s %s", linedest->line, linedest->destination);

      menu_cell_basic_draw(ctx, cell_layer, departure_title, linedest->departuretimes, NULL);

      break;
  }
}

static void menu_select_click_callback(MenuLayer* menu_layer, MenuIndex* cell_index, void* callback_context) {
  switch (cell_index->section) {
    case MENU_SECTION_MAIN:
      //refresh_departures();
    break;

  }
}

static void menu_select_long_click_callback(MenuLayer* menu_layer, MenuIndex* cell_index, void* callback_context) {

  realtime_transport_type_t *ttype = (realtime_transport_type_t *) callback_context;

  if (!layer_get_hidden(transport_type_to_loadinglayer_map[*ttype])) {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "No clicks allowed while loading layer is visible");
    return;
  }

  switch (cell_index->section) {
    case MENU_SECTION_MAIN:
      refresh_departures();
    break;

  }
}


static void window_load(Window *window) {

  int ttype = get_transport_type_from_window(window);

  MenuLayer *menu_layer = menu_layer_create_fullscreen(window);
  menu_layer_set_callbacks(menu_layer, (void *) &realtime_transport_types[ttype], (MenuLayerCallbacks){
    .get_num_sections = menu_get_num_sections_callback,
    .get_num_rows = menu_get_num_rows_callback,
    .get_header_height = menu_get_header_height_callback,
    .get_cell_height = menu_get_cell_height_callback,
    .draw_header = menu_draw_header_callback,
    .draw_row = menu_draw_row_callback,
    .select_click = menu_select_click_callback,
    .select_long_click = menu_select_long_click_callback
  });
  menu_layer_set_click_config_onto_window(menu_layer, window);
  menu_layer_add_to_window(menu_layer, window);

  // Set menu layer
  transport_type_to_menulayer_map[ttype] = menu_layer;

  // Set loading layer
  LoadingLayer *loading_layer = loading_layer_create(window);
  loading_layer_set_text(loading_layer, get_language_string(11));
  transport_type_to_loadinglayer_map[ttype] = loading_layer;  
}

static void window_unload(Window *window) {
  realtime_transport_type_t ttype = get_transport_type_from_window(window);
  menu_layer_destroy(transport_type_to_menulayer_map[ttype]);
  loading_layer_destroy(transport_type_to_loadinglayer_map[ttype]);
}

/**
 * Refresh automaticly
 */
static void app_timer_refresh_callback(void *data) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Automaticly Refreshing Departures");
  refresh_departures();

  // Continue refreshing
  refresh_timer = app_timer_register(REFRESH_TIMEOUT, app_timer_refresh_callback, NULL);
}

static void window_appear(Window *window) {
  refresh_timer = app_timer_register(REFRESH_TIMEOUT, app_timer_refresh_callback, NULL);
}

static void window_disappear(Window *window) {
  app_timer_cancel(refresh_timer);

  if (command_timeout_timer != NULL) {
    app_timer_cancel(command_timeout_timer);
    command_timeout_timer = NULL;
  }

}

/**
 * Notify the user on command timeout
 */
static void app_timer_command_timeout_callback(void *data) {
  realtime_transport_type_t *ttype = (realtime_transport_type_t *) data;
  command_timeout_timer = NULL;

  if (!window_stack_contains_window(transport_type_to_window_map[*ttype])) {
    return;
  }

  loading_layer_set_text(transport_type_to_loadinglayer_map[*ttype], get_language_string(15));
}

void create_departures_window(realtime_transport_type_t ttype) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Create departure window %d", ttype);
  Window *window = window_create();
  window_set_window_handlers(window, (WindowHandlers) {
    .load = window_load,
    .appear = window_appear,
    .unload = window_unload,
    .disappear = window_disappear
  });

  transport_type_to_window_map[ttype] = window;

}

void destroy_departures_window(realtime_transport_type_t ttype) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Destroy departure window %d", ttype);
  window_destroy(transport_type_to_window_map[ttype]);
}

void show_departures_window(char *stopid, realtime_transport_type_t ttype, bool animated) {
  window_stack_push(transport_type_to_window_map[ttype], animated);
  layer_show(transport_type_to_loadinglayer_map[ttype]);
  handle_get_departures(stopid, ttype);

  // Set command timeout timer
  command_timeout_timer = app_timer_register(COMMAND_TIMEOUT, app_timer_command_timeout_callback, (void *) &realtime_transport_types[ttype]);
}