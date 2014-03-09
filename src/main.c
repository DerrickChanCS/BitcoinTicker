#include <pebble.h>
  
Window* window;

TextLayer *currency_layer, *last_layer, *bid_layer, *time_layer;
char last_buffer[64], bid_buffer[64], time_buffer[32];

enum {
  KEY_LAST = 0,
  KEY_BID = 1,
};

void process_tuple(Tuple *t)
{
  //Get key
  int key = t->key;
 
  //Get double value, if present
  int value = t->value->int32;
 
  //Decide what to do
  switch(key) {
    case KEY_LAST:
      //Last received
      snprintf(last_buffer, sizeof("100000000000000"), "Last: %d", value);
      text_layer_set_text(last_layer, (char*) &last_buffer);
      break;
    case KEY_BID:
      //Bid received
      snprintf(bid_buffer, sizeof("100000000000000"), "Bid: %d", value);
      text_layer_set_text(bid_layer, (char*) &bid_buffer);
      break;
  }
 
  //Set time this update came in
  time_t temp = time(NULL);  
  struct tm *tm = localtime(&temp);
  strftime(time_buffer, sizeof("Last updated: XX:XX"), "Last updated: %H:%M", tm);
  text_layer_set_text(time_layer, (char*) &time_buffer);
}


static TextLayer* init_text_layer(GRect location, GColor colour, GColor background, const char *res_id, GTextAlignment alignment)
{
  TextLayer *layer = text_layer_create(location);
  text_layer_set_text_color(layer, colour);
  text_layer_set_background_color(layer, background);
  text_layer_set_font(layer, fonts_get_system_font(res_id));
  text_layer_set_text_alignment(layer, alignment);
 
  return layer;
}

void window_load(Window *window)
{
	currency_layer = init_text_layer(GRect(0, 0, 144, 154), GColorBlack, GColorClear, "RESOURCE_ID_BITHAM_42_BOLD", GTextAlignmentCenter);
	text_layer_set_text(currency_layer, "Bitcoin");
	layer_add_child(window_get_root_layer(window), text_layer_get_layer(currency_layer));

	last_layer = init_text_layer(GRect(0, 60, 144, 250), GColorBlack, GColorClear, "RESOURCE_ID_GOTHIC_28", GTextAlignmentCenter);
	text_layer_set_text(last_layer, "Last: N/A");
	layer_add_child(window_get_root_layer(window), text_layer_get_layer(last_layer));

	bid_layer = init_text_layer(GRect(0, 85, 144, 250), GColorBlack, GColorClear, "RESOURCE_ID_GOTHIC_28", GTextAlignmentCenter);
	text_layer_set_text(bid_layer, "Bid: N/A");
	layer_add_child(window_get_root_layer(window), text_layer_get_layer(bid_layer));

	time_layer = init_text_layer(GRect(0, 140, 144, 100), GColorBlack, GColorClear, "RESOURCE_ID_GOTHIC_18", GTextAlignmentCenter);
	text_layer_set_text(time_layer, "Last updated: N/A");
	layer_add_child(window_get_root_layer(window), text_layer_get_layer(time_layer));
}

void window_unload(Window *window)
{
  text_layer_destroy(currency_layer);
  text_layer_destroy(last_layer);
  text_layer_destroy(bid_layer);
  text_layer_destroy(time_layer); 
}

static void in_received_handler(DictionaryIterator *iter, void *context) 
{
  //Get data
  Tuple *t = dict_read_first(iter);
  if(t)
  {
    process_tuple(t);
  }
   
  //Get next
  while(t != NULL)
  {
    t = dict_read_next(iter);
    if(t)
    {
      process_tuple(t);
    }
  }
}
  
void init()
{
  window = window_create();
  WindowHandlers handlers = {
    .load = window_load,
    .unload = window_unload
  };
  window_set_window_handlers(window, handlers);
 
  //Register AppMessage events
  app_message_register_inbox_received(in_received_handler);           
  app_message_open(512, 512);    //Large input and output buffer sizes
  
  window_stack_push(window, true);
}
  
void deinit()
{
  window_destroy(window);
}
  
int main(void)
{
  init();
  app_event_loop();
  deinit();
}