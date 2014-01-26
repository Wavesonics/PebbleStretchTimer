#include <pebble.h>
#include "itoa.h"
	
Window *window;
TextLayer *event_name_text_layer;
TextLayer *event_time_text_layer;

const int8_t EVENT_TYPE_REST = 0;
const int8_t EVENT_TYPE_STRETCH = 1;

typedef struct
{
	int8_t type;
	char* name;
	int32_t duration;
} Event;

int curEventTime;

Event g_events[] = {
						{ 0, "Get ready...", 5 },
		
						{ 1, "Gastroc: Left", 30 },
						{ 0, "Rest", 5 },
						{ 1, "Gastroc: Right", 30 },
						{ 0, "Rest", 5 },
							
						{ 1, "Soleus: Left", 30 },
						{ 0, "Rest", 5 },
						{ 1, "Soleus: Right", 30 },
						{ 0, "Rest", 5 },
							
						{ 1, "Climber's: Left", 30 },
						{ 0, "Rest", 5 },
						{ 1, "Climber's: Right", 30 },
						{ 0, "Rest", 5 },
						
						{ 1, "Hip Flexor: Left", 30 },
						{ 0, "Rest", 5 },
						{ 1, "Hip Flexor: Right", 30 },
						{ 0, "Rest", 5 },
						
						{ 0, "Rest", 10 },
							
						{ 1, "Leg Hang: Left", 30 },
						{ 0, "Rest", 10 },
						{ 1, "Leg Hang: Right", 30 },
						{ 0, "Rest", 5 },
						
						{ 0, "Rest", 10 },
						
						{ 1, "Prone Hip Rotation: Left", 30 },
						{ 0, "Rest", 10 },
						{ 1, "Prone Hip Rotation", 30 }
					};
int g_currentEvent = -1;

int num_events()
{
	return sizeof( g_events ) / sizeof( Event );
}

void start_next_event()
{
	if( g_currentEvent < num_events() )
	{
		++g_currentEvent;
		
		curEventTime = -1;

		text_layer_set_text(event_name_text_layer, g_events[g_currentEvent].name);
		text_layer_set_text(event_time_text_layer, itoa(0));
	}
}

void tick_callback(struct tm *tick_time, TimeUnits units_changed)
{
	APP_LOG( APP_LOG_LEVEL_DEBUG, "Tick callback" );
	
	if( g_currentEvent >= 0 && g_currentEvent < num_events() )
	{
		++curEventTime;
		if( curEventTime <= g_events[g_currentEvent].duration )
		{
			text_layer_set_text(event_time_text_layer, itoa(g_events[g_currentEvent].duration-curEventTime));
		}
		else
		{
			switch(g_events[g_currentEvent].type)
			{
				case 0:
					vibes_short_pulse();
				break;
				case 1:
					vibes_long_pulse();
				break;
			}
			
			start_next_event();
		}
	}
	else
	{
		text_layer_set_text(event_name_text_layer, "Done!");
		text_layer_set_text(event_time_text_layer, "");
	}
}

void window_load( Window *window )
{
	/*
	// Initialize the action bar:
	actionBar = action_bar_layer_create();
	// Associate the action bar with the window:
	action_bar_layer_add_to_window( actionBar, window );
	// Set the click config provider:
	action_bar_layer_set_click_config_provider( actionBar, click_config_provider );
	
	// Set the icons:
	bitmapRing = gbitmap_create_with_resource( RESOURCE_ID_IMAGE_RING_ICON );
	bitmapSilence = gbitmap_create_with_resource( RESOURCE_ID_IMAGE_SILENCE_ICON );
	
	// Set the icons:
	action_bar_layer_set_icon( actionBar, BUTTON_ID_UP, bitmapRing );
	action_bar_layer_set_icon( actionBar, BUTTON_ID_DOWN, bitmapSilence );
	*/
}

void window_unload(Window *window)
{
	/*
	gbitmap_destroy( bitmapRing );
	gbitmap_destroy( bitmapSilence );
	*/
	
	text_layer_destroy( event_name_text_layer );
	text_layer_destroy( event_time_text_layer );
	window_destroy( window );
}

void setupUi( Window* window )
{
	Layer *window_layer = window_get_root_layer(window);
	GRect windowFrame = layer_get_frame(window_layer);
	int16_t aThird = windowFrame.size.h / 3;
	
	event_name_text_layer = text_layer_create(GRect(0, 0, windowFrame.size.w, aThird));
	text_layer_set_text(event_name_text_layer, "Event name");
	text_layer_set_font(event_name_text_layer, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));
	layer_add_child(window_layer, text_layer_get_layer(event_name_text_layer));
	
	event_time_text_layer = text_layer_create(GRect(0, aThird, windowFrame.size.w, aThird));
	text_layer_set_text(event_time_text_layer, "0");
	text_layer_set_font(event_time_text_layer, fonts_get_system_font(FONT_KEY_BITHAM_42_BOLD));
	layer_add_child(window_layer, text_layer_get_layer(event_time_text_layer));
}

void handle_init(void) {
	window = window_create();
	
	setupUi(window);
	
	WindowHandlers handlers = {
		.load = window_load,
		.unload = window_unload
	};
	
	window_set_window_handlers(window, handlers);
	
	window_stack_push(window, true /* Animated */);
	
	tick_timer_service_subscribe( SECOND_UNIT, tick_callback );
	
	start_next_event();
}

void handle_deinit(void) {
	tick_timer_service_unsubscribe();
	
	text_layer_destroy(event_name_text_layer);
	text_layer_destroy(event_time_text_layer);
	window_destroy(window);
}

int main(void) {
	APP_LOG( APP_LOG_LEVEL_DEBUG, "StretchTimer started" );
	
	handle_init();
	app_event_loop();
	handle_deinit();
	
	APP_LOG( APP_LOG_LEVEL_DEBUG, "StretchTimer closing..." );
	
	return 0;
}
