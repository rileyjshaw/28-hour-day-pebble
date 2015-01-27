#include <pebble.h>
  
static Window *s_main_window;
static TextLayer *s_zzz_layer;
static TextLayer *s_time_layer;
static TextLayer *s_day_layer;

const char *days[6] = {"one", "two", "three", "four", "five", "six"};

static void update_time() {
  time_t temp = time(NULL); 
  struct tm *tick_time = localtime(&temp);
  
  static char time[] = "27:59";
  static char day[] = "day three";
  
  int min = tick_time->tm_min;
  int hour = tick_time->tm_hour;
  int wday = tick_time->tm_wday;
  
  // total hours elapsed so far this week
  hour = 24 * wday + hour;
  
  wday = hour / 28;  
  hour = hour % 28;
  
  if (hour > 19) {
    text_layer_set_text(s_zzz_layer, "Zzz");
  }

  if(clock_is_24h_style() == false) {
    hour %= 14;
  }

  snprintf(time, sizeof(time), "%d:%02d", hour, min);
  
  // get the strings in order
  strcpy(day, "day ");
  strcat(day, days[wday]);

  text_layer_set_text(s_time_layer, time);
  text_layer_set_text(s_day_layer, day);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  update_time();
}

static void main_window_load (Window *window) {
  // zzz layer
  s_zzz_layer = text_layer_create(GRect(0, 31, 144, 24));
  text_layer_set_background_color(s_zzz_layer, GColorClear);
  text_layer_set_text_color(s_zzz_layer, GColorBlack);
  text_layer_set_font(s_zzz_layer, fonts_get_system_font(FONT_KEY_GOTHIC_18));
  text_layer_set_text_alignment(s_zzz_layer, GTextAlignmentCenter);
  
  // time layer
  s_time_layer = text_layer_create(GRect(0, 50, 144, 42));
  text_layer_set_background_color(s_time_layer, GColorClear);
  text_layer_set_text_color(s_time_layer, GColorBlack);
  text_layer_set_font(s_time_layer, fonts_get_system_font(FONT_KEY_BITHAM_42_BOLD));
  text_layer_set_text_alignment(s_time_layer, GTextAlignmentCenter);
  
  // day layer
  s_day_layer = text_layer_create(GRect(0, 98, 144, 24));
  text_layer_set_background_color(s_day_layer, GColorClear);
  text_layer_set_text_color(s_day_layer, GColorBlack);
  text_layer_set_font(s_day_layer, fonts_get_system_font(FONT_KEY_GOTHIC_18));
  text_layer_set_text_alignment(s_day_layer, GTextAlignmentCenter);
  
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_zzz_layer));
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_time_layer));
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_day_layer));
  update_time();
}

static void main_window_unload (Window *window) {
  text_layer_destroy(s_time_layer);
}

static void init () {
  s_main_window = window_create();
  
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload
  });
  
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
  window_stack_push(s_main_window, true);
}

static void deinit () {
  window_destroy(s_main_window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}
