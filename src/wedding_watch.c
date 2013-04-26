#include "pebble_os.h"
#include "pebble_app.h"
#include "pebble_fonts.h"
#include "num2words.h"
#include "ctype.h"


#define MY_UUID { 0x59, 0x47, 0x88, 0x13, 0xBE, 0xC8, 0x41, 0xF0, 0x9C, 0xDA, 0x77, 0x39, 0x88, 0x3C, 0xBF, 0xE3 }
PBL_APP_INFO(MY_UUID,
             "Wedding Time", "Alex Goodey",
             1, 0, /* App version */
             DEFAULT_MENU_ICON,
             APP_INFO_WATCH_FACE);

#define BUFFER_SIZE 86
  char countText[] = "000 days to go";



void line_layer_update_callback(Layer *me, GContext* ctx) {
    (void)me;
    
    graphics_context_set_stroke_color(ctx, GColorWhite);
    
    graphics_draw_line(ctx, GPoint(0, 33), GPoint(144, 33));
    graphics_draw_line(ctx, GPoint(0, 34), GPoint(144, 34));
    
}

static struct CommonWordsData {
  TextLayer label;
  Window window;
  char buffer[BUFFER_SIZE];
  Layer line_layer;
  TextLayer text_date_layer;
} s_data;

// transforme un entier en char* (8 characters max)
static void itoa2(int num, char* buffer) {
    const char digits[10] = "0123456789";
    int iSize;
    
    if(num > 999) {
        buffer[0] = '9';
        buffer[1] = '9';
        buffer[2] = '9';
		iSize = 3;
        
    } else if(num > 99) {
        
        buffer[0] = digits[num / 100];
        if(num%100 > 9) {
            buffer[1] = digits[(num%100) / 10];
        } else {
            buffer[1] = '0';
        }
        buffer[2] = digits[num%10];
        iSize = 3;
    }
    
    else if(num > 9) {
        buffer[0] = digits[num / 10];
        buffer[1] = digits[num % 10];
        iSize = 2;
        buffer[7] = ' ';
    } else {
        buffer[0] = digits[num % 10];
        iSize = 1;
        buffer[6] = ' ';
        buffer[7] = ' ';
    }
    
    buffer[iSize] = ' ';
    buffer[iSize+1] = 'd';
    buffer[iSize+2] = 'a';
    buffer[iSize+3] = 'y';
    if(num < 2)
    	{
    	buffer[iSize+4] = ' ';
    	buffer[iSize+5] = 't';
    	buffer[iSize+6] = 'o';
    	buffer[iSize+7] = ' ';
    	buffer[iSize+8] = 'g';
    	buffer[iSize+9] = 'o';
    	buffer[iSize+10] = ' ';
    	}
    else
    	{
    	buffer[iSize+4] = 's';
     	buffer[iSize+5] = ' ';
    	buffer[iSize+6] = 't';
    	buffer[iSize+7] = 'o';
    	buffer[iSize+8] = ' ';
    	buffer[iSize+9] = 'g';
    	buffer[iSize+10] = 'o';
   	}
    		
}


static void update_time(PblTm* t) {
  static char count_text[] = "000 days to go";
  int iTargetDate = 291; // 19 October
  int iyDay = t->tm_yday;
  int iNumDays = iTargetDate - iyDay;
  if(iNumDays < 0)
    {
	iNumDays = 0;
    }
  fuzzy_time_to_words(t->tm_hour, t->tm_min, s_data.buffer, BUFFER_SIZE);
  text_layer_set_text(&s_data.label, s_data.buffer);  
  
  itoa2(iNumDays, &count_text[0]);
  text_layer_set_text(&s_data.text_date_layer, count_text);

}

static void handle_minute_tick(AppContextRef app_ctx, PebbleTickEvent* e) {
  update_time(e->tick_time);
}

static void handle_init(AppContextRef ctx) {
   
 (void) ctx;

  window_init(&s_data.window, "Wedding Time");
  const bool animated = true;
  window_stack_push(&s_data.window, animated);

  window_set_background_color(&s_data.window, GColorBlack);
  GFont timefont = fonts_get_system_font(FONT_KEY_GOTHAM_30_BLACK);
  GFont datefont = fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD);

    
    text_layer_init(&s_data.text_date_layer, s_data.window.layer.frame);
    text_layer_set_text_color(&s_data.text_date_layer, GColorWhite);
    text_layer_set_background_color(&s_data.text_date_layer, GColorClear);
    layer_set_frame(&s_data.text_date_layer.layer, GRect(0, 0, 144, 32));
    text_layer_set_font(&s_data.text_date_layer, datefont);
    layer_add_child(&s_data.window.layer, &s_data.text_date_layer.layer);
    
  text_layer_init(&s_data.label, GRect(0, 35, s_data.window.layer.frame.size.w - 0, s_data.window.layer.frame.size.h - 35));
  text_layer_set_background_color(&s_data.label, GColorBlack);
  text_layer_set_text_color(&s_data.label, GColorWhite);
  text_layer_set_font(&s_data.label, timefont);
  layer_add_child(&s_data.window.layer, &s_data.label.layer);
    
    layer_init(&s_data.line_layer, s_data.window.layer.frame);
    s_data.line_layer.update_proc = &line_layer_update_callback;
    layer_add_child(&s_data.window.layer, &s_data.line_layer);
  

  PblTm t;
  get_time(&t);
  update_time(&t);

}



void pbl_main(void *params) {
  PebbleAppHandlers handlers = {
    .init_handler = &handle_init,

    .tick_info = {
      .tick_handler = &handle_minute_tick,
      .tick_units = MINUTE_UNIT
    }

  };
  app_event_loop(params, &handlers);
}