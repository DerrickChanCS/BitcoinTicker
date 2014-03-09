#include <pebble.h>

Window *window;
TextLayer *currency_layer, *price_layer, *time_layer;
char price_buffer[64], time_buffer[32];

enum {
  KEY_PRICE = 0,
};

void process_tuple(Tuple *t) {
  int key = t->key;
  
  // Get integer value, if present
  int value = t->value->int32;
 
  switch(key) {
    // Price received
    case KEY_PRICE:
      snprintf(price_buffer, sizeof("Price: XX \u00B0C"), "Price: %d \u00B0C", value);
      text_layer_set_text(price_layer, (char*) &price_buffer);
      break;
  }
 
  //Set time this update came in
  time_t temp = time(NULL);  
  struct tm *tm = localtime(&temp);
  strftime(time_buffer, sizeof("Last updated: XX:XX"), "Last updated: %H:%M", tm);
  text_layer_set_text(time_layer, (char*) &time_buffer);
}

// clicking stuffs
void up_click_handler(ClickRecognizerRef recognizer, void *context) {
  text_layer_set_background_color(price_layer, GColorBlack);
}
   
void down_click_handler(ClickRecognizerRef recognizer, void *context) {
  text_layer_set_background_color(price_layer, GColorWhite);
}
 
void select_click_handler(ClickRecognizerRef recognizer, void *context) { 
  vibes_short_pulse();
}

static void click_config_provider(void *context) {
  window_single_click_subscribe(BUTTON_ID_UP, up_click_handler);
  window_single_click_subscribe(BUTTON_ID_DOWN, down_click_handler);
  window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
}

static void in_received_handler(DictionaryIterator *iter, void *context) {
  
  (void) context;
	
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

void send_int(uint8_t key, uint8_t cmd) {
    DictionaryIterator *iter;
    app_message_outbox_begin(&iter);
 	
 	  Tuplet value = TupletInteger(key, cmd);
 	  dict_write_tuplet(iter, &value);
 	
   	app_message_outbox_send();
}

void tick_callback(struct tm *tick_time, TimeUnits units_changed) {
  // Every one minute
	if(tick_time->tm_min % 1 == 0)
	{
		//Send an arbitrary message, the response will be handled by in_received_handler()
		send_int(5, 5);
	}
}

void handle_init(void) {
	// Create a window and text layer
	window = window_create();
	currency_layer = text_layer_create(GRect(0, 0, 144, 154));
  price_layer = text_layer_create(GRect(0, 65, 144, 250));
  time_layer = text_layer_create(GRect(0, 120, 144, 100));
	
	text_layer_set_text(currency_layer, "Bitcoin");
	text_layer_set_font(currency_layer, fonts_get_system_font(FONT_KEY_BITHAM_42_BOLD));
  text_layer_set_background_color(currency_layer, GColorClear);
	text_layer_set_text_alignment(currency_layer, GTextAlignmentCenter);
  
  text_layer_set_text(price_layer, "N/A");
	text_layer_set_font(price_layer, fonts_get_system_font(FONT_KEY_GOTHIC_28));
	text_layer_set_text_alignment(price_layer, GTextAlignmentCenter);
  
  text_layer_set_text(time_layer, "Last Updated: ");
	text_layer_set_font(time_layer, fonts_get_system_font(FONT_KEY_GOTHIC_18));
	text_layer_set_text_alignment(time_layer, GTextAlignmentCenter);
	
	// Add the text layer to the window
	layer_add_child(window_get_root_layer(window), text_layer_get_layer(currency_layer));
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(price_layer));
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(time_layer));
  
  // Register AppMessage events
	app_message_register_inbox_received(in_received_handler);					 
	app_message_open(512, 512);		//Large input and output buffer sizes
	
	//Register to receive minutely updates
	tick_timer_service_subscribe(MINUTE_UNIT, tick_callback);
  
  // Clicker
  window_set_click_config_provider(window, click_config_provider);
  
  // Push the window
	window_stack_push(window, true);
}

void handle_deinit(void) {
	// Destroy the text layer
	text_layer_destroy(currency_layer);
  text_layer_destroy(price_layer);
  text_layer_destroy(time_layer);
	
	// Destroy the window
	window_destroy(window);
  
  tick_timer_service_unsubscribe();
}

int main(void) {
	handle_init();
	app_event_loop();
	handle_deinit();
}
