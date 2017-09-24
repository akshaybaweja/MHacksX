#include <pebble.h>

static Window *main_win;
static TextLayer *txt_layer;
static Layer *win_layer;

bool test_mode = true;

void send_int(uint8_t key, uint8_t cmd){
    DictionaryIterator *dict_iter;
    app_message_outbox_begin(&dict_iter);
    Tuplet value = TupletInteger(key, cmd);
    dict_write_tuplet(dict_iter, &value);    
    app_message_outbox_send();
}

void test_int(void* data){
  send_int(100, 100);
}

void test_conn(){
  if (!test_mode) {
    return;
  }
  char* msg = "Testing";
  
  text_layer_set_text(txt_layer, msg);
  
  test_int(NULL);
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Ye toh ho gya bc");
}

static void select_click_handler(ClickRecognizerRef recognizer, void *context) {
  test_conn();
  APP_LOG(APP_LOG_LEVEL_DEBUG, "select button dabb gya bc o_O");
}

static void back_click_handler(ClickRecognizerRef recognizer, void *context) {
  if (test_mode) {
    window_stack_pop(true);
  } else {
    test_mode = true;
    test_conn();
  }
  APP_LOG(APP_LOG_LEVEL_DEBUG, "back button dabb gya bc o_O");
}

static void click_config_provider(void *context) {
  window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
  window_single_click_subscribe(BUTTON_ID_BACK, back_click_handler);
  APP_LOG(APP_LOG_LEVEL_DEBUG, "ullu k pathe handled :P");
}

static void window_load(Window *window) {
  win_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(win_layer);
  
  txt_layer = text_layer_create(bounds);
  text_layer_set_text(txt_layer, "Testing 1 2 3");
  
  text_layer_set_font(txt_layer, fonts_get_system_font(FONT_KEY_BITHAM_42_BOLD));
  text_layer_set_text_alignment(txt_layer, GTextAlignmentCenter);
  
  text_layer_enable_screen_text_flow_and_paging(txt_layer, 10);
  
  GSize max_size = text_layer_get_content_size(txt_layer);
  text_layer_set_size(txt_layer, max_size);
  layer_add_child(win_layer, text_layer_get_layer(txt_layer));  
  
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Window Loaded");
}

static void main_window_unload(Window *window) {
  text_layer_destroy(txt_layer);
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Window Unloaded");
}

static void out_failed_handler(DictionaryIterator *iter, AppMessageResult reason, void *context) {
  char* str = "Connection to Android App Failed \nPress Select to Test Again";
  text_layer_set_text(txt_layer, str);
//   GSize max_size = text_layer_get_content_size(txt_layer);
//   text_layer_set_size(txt_layer, GSize(max_size.w, max_size.h + 4));
  text_layer_set_font(txt_layer, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));
  text_layer_set_text_alignment(txt_layer, GTextAlignmentCenter);
  
}

static void out_sent_handler(DictionaryIterator *iter, void *context) {
  char* str = "Connected to Android App";
  text_layer_set_text(txt_layer, str);
//   GSize max_size = text_layer_get_content_size(txt_layer);
//   text_layer_set_size(txt_layer, GSize(max_size.w, max_size.h + 4));
  text_layer_set_font(txt_layer, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));
  text_layer_set_text_alignment(txt_layer, GTextAlignmentCenter);
  
}

static void in_received_handler(DictionaryIterator *iter, void *context) 
{
  static const uint32_t pattern[] = {300,100,200,200,200,130,300,100,300,100,10};
  VibePattern pat = {
    .durations = pattern,
    .num_segments = ARRAY_LENGTH(pattern),
  };

  
  Tuple *t = dict_read_first(iter);
    if(t)
    {
        vibes_enqueue_custom_pattern(pat);
        char* str = (char*)t->value->cstring;
        text_layer_set_text(txt_layer, str);
        GSize max_size = text_layer_get_content_size(txt_layer);
        text_layer_set_size(txt_layer, GSize(max_size.w, max_size.h + 4));
        test_mode = false;
    }   
}

static void init(void) {
  main_win = window_create();
  window_set_click_config_provider(main_win, click_config_provider);
  window_set_window_handlers(main_win, (WindowHandlers) {
    .load = window_load,
    .unload = main_window_unload,
  });
  app_message_register_inbox_received(in_received_handler);    
  app_message_register_outbox_failed(out_failed_handler);
  app_message_register_outbox_sent(out_sent_handler);
  app_message_open(512, 512);
  
  window_stack_push(main_win, true);
  
  test_conn();
  
  APP_LOG(APP_LOG_LEVEL_DEBUG, "init done babes");
}
 
static void deinit(void) {
  window_destroy(main_win);
}

enum {
    KEY_BUTTON_EVENT = 0,
    BUTTON_EVENT_UP = 1,
    BUTTON_EVENT_DOWN = 2,
    BUTTON_EVENT_SELECT = 3
};

int main(void) {
  init();
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Done initializing, pushed window: %p", main_win);
  app_event_loop();
  deinit();
}