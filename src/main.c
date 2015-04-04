/*

   "Classic" Digital Watch Pebble App

 */

// Standard includes
#include "pebble.h"
#include "sqlcode.h"

// App-specific data
Window *window; // All apps must have at least one window
TextLayer *time_layer; // The clock
TextLayer *sql_layer; // The text

// Called once per minute
static void handle_minute_tick(struct tm* tick_time, TimeUnits units_changed) {

  static char time_text[] = "00:00"; // Needs to be static because it's used by the system later.

  strftime(time_text, sizeof(time_text), "%R", tick_time);

  static char time_sql[] = "00000"; // Needs to be static because it's used by the system later.
  strftime(time_sql, sizeof(time_sql), "-%l%M", tick_time);

  static char sql_layer_text[250];
  strcpy(sql_layer_text, sqlcode_get_message(time_sql));
  if (sqlcode_change_time() == 1)
  {
    strcpy(time_text, time_sql);
  }
  
  text_layer_set_text(time_layer, time_text);
  text_layer_set_text(sql_layer, sql_layer_text);

}


// Handle the start-up of the app
static void do_init(void) {

  // Create our app's base window
  window = window_create();
  window_stack_push(window, true);
  window_set_background_color(window, GColorBlack);

  // Init the text layer used to show the time
  time_layer = text_layer_create(GRect(0, 0, 144 /* width */, 46 /* height */));
  text_layer_set_text_color(time_layer, GColorWhite);
  text_layer_set_background_color(time_layer, GColorClear);
  text_layer_set_font(time_layer, fonts_get_system_font(FONT_KEY_BITHAM_42_BOLD));
  text_layer_set_text_alignment(time_layer, GTextAlignmentCenter);

  // Init the text layer used to show the time
  sql_layer = text_layer_create(GRect(5, 46, 144-10 /* width */, 168-46 /* height */));
  text_layer_set_text_color(sql_layer, GColorWhite);
  text_layer_set_background_color(sql_layer, GColorClear);
  text_layer_set_font(sql_layer, fonts_get_system_font(FONT_KEY_GOTHIC_14_BOLD));

  // Ensures time is displayed immediately (will break if NULL tick event accessed).
  // (This is why it's a good idea to have a separate routine to do the update itself.)
  time_t now = time(NULL);
  struct tm *current_time = localtime(&now);
  handle_minute_tick(current_time, MINUTE_UNIT);
  tick_timer_service_subscribe(MINUTE_UNIT, &handle_minute_tick);

  layer_add_child(window_get_root_layer(window), text_layer_get_layer(time_layer));
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(sql_layer));
}

static void do_deinit(void) {
  text_layer_destroy(time_layer);
  text_layer_destroy(sql_layer);
  window_destroy(window);
}

// The main event/run loop for our app
int main(void) {
  do_init();
  app_event_loop();
  do_deinit();
}