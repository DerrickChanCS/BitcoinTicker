#include "pebble.h"
  
static Window *window;
static Window *wins[3];
static SimpleMenuLayer *sml;
static SimpleMenuSection menu_sec[1];
static SimpleMenuItem first_it[3];
static GBitmap *bitcoin_img, *dogecoin_img, *litecoin_img;

TextLayer *bitcoin_layer, *dogecoin_layer, *litecoin_layer, *time_layer1, *time_layer2, *time_layer3;
TextLayer *bitcoin_price, *dogecoin_price, *litecoin_price;
char bitcoin_buffer[32], dogecoin_buffer[32], litecoin_buffer[32], time_buffer1[32], time_buffer2[32], time_buffer3[32];

enum {
  KEY_BPRICE = 0,
  KEY_DPRICE = 1,
  KEY_LPRICE = 2,
};

static void process_tuple(Tuple *t) {
  //Get key
  int key = t->key;
 
  //Get double value, if present
  char string_value[32];
  strcpy(string_value, t->value->cstring);
 
  //Decide what to do
  switch(key) {
    case KEY_BPRICE:
      // Bitcoin received
      snprintf(bitcoin_buffer, sizeof("Dcoin: couldbereallylongname"), "$%s", string_value);
      text_layer_set_text(bitcoin_price, (char*) &bitcoin_buffer);
    
      // Set time this update came in
      time_t temp1 = time(NULL);  
      struct tm *tm1 = localtime(&temp1);
      strftime(time_buffer1, sizeof("Last updated: XX:XX"), "Last updated: %H:%M", tm1);
      text_layer_set_text(time_layer1, (char*) &time_buffer1);
    
      break;
    case KEY_DPRICE:
      // Dogecoin received
      snprintf(dogecoin_buffer, sizeof("Dogecoin: couldbereallylongname"), "$%s", string_value);
      text_layer_set_text(dogecoin_price, (char*) &dogecoin_buffer);
    
      // Set time this update came in
      time_t temp2 = time(NULL);  
      struct tm *tm2 = localtime(&temp2);
      strftime(time_buffer2, sizeof("Last updated: XX:XX"), "Last updated: %H:%M", tm2);
      text_layer_set_text(time_layer2, (char*) &time_buffer2);
    
      break;
    case KEY_LPRICE:
      // Litecoin received
      snprintf(litecoin_buffer, sizeof("Litecoin: couldbereallylongname"), "$%s", string_value);
      text_layer_set_text(litecoin_price, (char*) &litecoin_buffer);
    
      // Set time this update came in
      time_t temp3 = time(NULL);  
      struct tm *tm3 = localtime(&temp3);
      strftime(time_buffer3, sizeof("Last updated: XX:XX"), "Last updated: %H:%M", tm3);
      text_layer_set_text(time_layer3, (char*) &time_buffer3);
    
      break;
  }
}

static TextLayer* init_text_layer(GRect location, GColor colour, GColor background, const char *res_id, GTextAlignment alignment) {
  TextLayer *layer = text_layer_create(location);
  text_layer_set_text_color(layer, colour);
  text_layer_set_background_color(layer, background);
  text_layer_set_font(layer, fonts_get_system_font(res_id));
  text_layer_set_text_alignment(layer, alignment);
 
  return layer;
}

static void init_layers(void) {
	bitcoin_layer = init_text_layer(GRect(0, 0, 144, 250), GColorBlack, GColorClear, "RESOURCE_ID_BITHAM_42_BOLD", GTextAlignmentCenter);
	text_layer_set_text(bitcoin_layer, "Bitcoin");
	layer_add_child(window_get_root_layer(wins[0]), text_layer_get_layer(bitcoin_layer));

	dogecoin_layer = init_text_layer(GRect(0, 0, 144, 250), GColorBlack, GColorClear, "RESOURCE_ID_GOTHIC_28_BOLD", GTextAlignmentCenter);
	text_layer_set_text(dogecoin_layer, "Dogecoin\n(10000)");
	layer_add_child(window_get_root_layer(wins[1]), text_layer_get_layer(dogecoin_layer));
  
  litecoin_layer = init_text_layer(GRect(0, 0, 144, 250), GColorBlack, GColorClear, "RESOURCE_ID_BITHAM_30_BLACK", GTextAlignmentCenter);
	text_layer_set_text(litecoin_layer, "Litecoin");
	layer_add_child(window_get_root_layer(wins[2]), text_layer_get_layer(litecoin_layer));

	time_layer1 = init_text_layer(GRect(0, 120, 144, 100), GColorBlack, GColorClear, "RESOURCE_ID_GOTHIC_18", GTextAlignmentCenter);
	text_layer_set_text(time_layer1, "Last updated: N/A");
	layer_add_child(window_get_root_layer(wins[0]), text_layer_get_layer(time_layer1));
  
  time_layer2 = init_text_layer(GRect(0, 120, 144, 100), GColorBlack, GColorClear, "RESOURCE_ID_GOTHIC_18", GTextAlignmentCenter);
	text_layer_set_text(time_layer2, "Last updated: N/A");
  layer_add_child(window_get_root_layer(wins[1]), text_layer_get_layer(time_layer2));
  
  time_layer3 = init_text_layer(GRect(0, 120, 144, 100), GColorBlack, GColorClear, "RESOURCE_ID_GOTHIC_18", GTextAlignmentCenter);
	text_layer_set_text(time_layer3, "Last updated: N/A");
  layer_add_child(window_get_root_layer(wins[2]), text_layer_get_layer(time_layer3));
  
  bitcoin_price = init_text_layer(GRect(0, 60, 144, 250), GColorBlack, GColorClear, "RESOURCE_ID_GOTHIC_24", GTextAlignmentCenter);
  text_layer_set_text(bitcoin_price, "$N/A");
  layer_add_child(window_get_root_layer(wins[0]), text_layer_get_layer(bitcoin_price));
  
  dogecoin_price = init_text_layer(GRect(0, 60, 144, 250), GColorBlack, GColorClear, "RESOURCE_ID_GOTHIC_24", GTextAlignmentCenter);
  text_layer_set_text(dogecoin_price, "$N/A");
  layer_add_child(window_get_root_layer(wins[1]), text_layer_get_layer(dogecoin_price));
  
  litecoin_price = init_text_layer(GRect(0, 60, 144, 250), GColorBlack, GColorClear, "RESOURCE_ID_GOTHIC_24", GTextAlignmentCenter);
  text_layer_set_text(litecoin_price, "$N/A");
  layer_add_child(window_get_root_layer(wins[2]), text_layer_get_layer(litecoin_price));
}

static void handle_wins_init(void){
  wins[0] = window_create();
  wins[1] = window_create();
  wins[2] = window_create();
  
  init_layers();
}

static void handle_wins_deinit(void){
  text_layer_destroy(bitcoin_layer);
  text_layer_destroy(dogecoin_layer);
  text_layer_destroy(litecoin_layer);
  text_layer_destroy(time_layer1); 
  text_layer_destroy(time_layer2);
  text_layer_destroy(time_layer3);
  text_layer_destroy(bitcoin_price);
  text_layer_destroy(dogecoin_price);
  text_layer_destroy(litecoin_price);
  
  window_destroy(wins[0]);
  window_destroy(wins[1]);
  window_destroy(wins[2]);
}

static void coin_select_cb(int index, void*ctx){
  window_stack_push(wins[index],true);
  layer_mark_dirty(simple_menu_layer_get_layer(sml));
}

static void in_received_handler(DictionaryIterator *iter, void *context) 
{
  
  //Get data
  Tuple *t = dict_read_first(iter);
  if(t != NULL)
  {
    process_tuple(t);
  }
  APP_LOG(APP_LOG_LEVEL_DEBUG, "In received handler");
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

static void send_int(uint8_t key, uint8_t cmd)
{
    DictionaryIterator *iter;
    app_message_outbox_begin(&iter);
     
    Tuplet value = TupletInteger(key, cmd);
    dict_write_tuplet(iter, &value);
     
    app_message_outbox_send();
}

static void tick_callback(struct tm *tick_time, TimeUnits units_changed)
{
    //Every minute
    if(tick_time->tm_min % 1 == 0)
    {
        //Send an arbitrary message, the response will be handled by in_received_handler()
        send_int(5, 5);
    }
}

static void window_init(Window *window){
  bitcoin_img = gbitmap_create_with_resource(RESOURCE_ID_LG_BIT);
  dogecoin_img = gbitmap_create_with_resource(RESOURCE_ID_LG_DOGE);
  litecoin_img = gbitmap_create_with_resource(RESOURCE_ID_LG_LITE);
  
  first_it[0] = (SimpleMenuItem){
    .title = "BitCoin",
    .callback = coin_select_cb,
    .icon = bitcoin_img,
  };
  first_it[1] = (SimpleMenuItem){
    .title = "DogeCoin",
    .callback = coin_select_cb,
    .icon = dogecoin_img,
  };
  first_it[2] = (SimpleMenuItem){
    .title = "LiteCoin",
    .callback = coin_select_cb,
    .icon = litecoin_img,
  };
  menu_sec[0] = (SimpleMenuSection){
    .num_items = 3,
    .items = first_it,
  };
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_frame(window_layer);
  
  sml = simple_menu_layer_create(bounds, window, menu_sec, 2, NULL);
  
  layer_add_child(window_layer, simple_menu_layer_get_layer(sml));
  
  handle_wins_init();
}

static void window_deinit(void) {
  handle_wins_deinit();
  simple_menu_layer_destroy(sml);
}

static void handle_init(void) {
  window = window_create();
  window_init(window);
  
  //Register AppMessage events
  app_message_register_inbox_received(in_received_handler);           
  app_message_open(512, 512);    //Large input and output buffer sizes
  
  //Register to receive minutely updates
  tick_timer_service_subscribe(MINUTE_UNIT, tick_callback);
  
  window_stack_push(window, true /* Animated */);
}

static void handle_deinit(void) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Hi im handeling deinit");
  gbitmap_destroy(bitcoin_img);
  gbitmap_destroy(dogecoin_img);
  gbitmap_destroy(litecoin_img);
  
  window_deinit();
  window_destroy(window);
  
  tick_timer_service_unsubscribe();
}

int main(void) {
  handle_init();
  app_event_loop();
  handle_deinit();
}