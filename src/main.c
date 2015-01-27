#include <pebble.h>
  
static const unsigned short int ZZZ_KEY = 0;

static AppSync s_sync;
static uint8_t s_sync_buffer[32];
  
static Window *s_main_window;
static TextLayer *s_zzz_layer;
static TextLayer *s_time_layer;
static TextLayer *s_day_layer;

const char *days[6] = {"one", "two", "three", "four", "five", "six"};
// TODO: hour is better for battery life if kept as static in update_time() 
int hour;
int zzz_toggle;

static void sync_changed_handler(const uint32_t key, const Tuple *new_tuple, const Tuple *old_tuple, void *context) {
  zzz_toggle = new_tuple->value->uint8 - 48;
  
  if (zzz_toggle == 1 && hour > 19) {
    text_layer_set_text(s_zzz_layer, "Zzz");
  } else text_layer_set_text(s_zzz_layer, "");
}

static void sync_error_handler(DictionaryResult dict_error, AppMessageResult app_message_error, void *context) {
  // TODO: Handle this elegantly
  APP_LOG(APP_LOG_LEVEL_ERROR, "Sync error...");
}

static void update_time() {
  time_t temp = time(NULL); 
  struct tm *tick_time = localtime(&temp);
  
  static char time[] = "27:59";
  static char day[] = "day three";
  
  int min = tick_time->tm_min;
  hour = tick_time->tm_hour;
  int wday = tick_time->tm_wday;
  
  // total hours elapsed so far this week
  hour = 24 * wday + hour;
  
  wday = hour / 28;  
  hour = hour % 28;
  
  // TODO: Shouldn't have to set this every minute
  if (zzz_toggle == 1 && hour > 19) {
    text_layer_set_text(s_zzz_layer, "Zzz");
  } else text_layer_set_text(s_zzz_layer, "");

  if(clock_is_24h_style() == false) {
    hour = (hour - 1) % 14 + 1;
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
  s_time_layer = text_layer_create(GRect(0, 50, 144, 44));
  text_layer_set_background_color(s_time_layer, GColorClear);
  text_layer_set_text_color(s_time_layer, GColorBlack);
  text_layer_set_font(s_time_layer, fonts_get_system_font(FONT_KEY_BITHAM_42_BOLD));
  text_layer_set_text_alignment(s_time_layer, GTextAlignmentCenter);
  
  // day layer
  s_day_layer = text_layer_create(GRect(0, 99, 144, 24));
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
  
  // settings sync
  zzz_toggle = persist_exists(ZZZ_KEY) ? persist_read_int(ZZZ_KEY) : 0;
  app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());

  Tuplet initial_values[] = {
    TupletInteger(ZZZ_KEY, zzz_toggle),
  };

  // Begin using AppSync
  app_sync_init(&s_sync, s_sync_buffer, sizeof(s_sync_buffer), initial_values, ARRAY_LENGTH(initial_values), sync_changed_handler, sync_error_handler, NULL);
  
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
  window_stack_push(s_main_window, true);
}

static void deinit () {
  persist_write_int(ZZZ_KEY, zzz_toggle);
  window_destroy(s_main_window);
  app_sync_deinit(&s_sync);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}
