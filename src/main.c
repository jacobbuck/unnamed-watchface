#include <pebble.h>

const int SPRITE_SIZE = 48;

static Window *s_main_window;

static BitmapLayer *s_number0_layer;
static BitmapLayer *s_number1_layer;
static BitmapLayer *s_number2_layer;
static BitmapLayer *s_number3_layer;

static GBitmap *s_numbers_bitmap;

static GBitmap *s_number0_bitmap;
static GBitmap *s_number1_bitmap;
static GBitmap *s_number2_bitmap;
static GBitmap *s_number3_bitmap;

static void draw_blank_to_layer(BitmapLayer *layer, GBitmap *bitmap) {
	bitmap = gbitmap_create_blank(GSize(SPRITE_SIZE, SPRITE_SIZE));
	bitmap_layer_set_compositing_mode(layer, GCompOpAssign);
	bitmap_layer_set_bitmap(layer, bitmap);
}

static void draw_number_to_layer(BitmapLayer *layer, GBitmap *bitmap, int number) {
	bitmap = gbitmap_create_as_sub_bitmap(s_numbers_bitmap, GRect(number * SPRITE_SIZE, 0, SPRITE_SIZE, SPRITE_SIZE));
	bitmap_layer_set_compositing_mode(layer, GCompOpAssignInverted);
	bitmap_layer_set_bitmap(layer, bitmap);
}

static void update_time() {
	// Get a tm structure
	time_t now = time(NULL);
	struct tm *tick_time = localtime(&now);

	int hour = tick_time->tm_hour;
	int min = tick_time->tm_min;

	if (clock_is_24h_style() == false && hour > 12) {
		hour -= 12;
	}

	gbitmap_destroy(s_number0_bitmap);
	gbitmap_destroy(s_number1_bitmap);
	gbitmap_destroy(s_number2_bitmap);
	gbitmap_destroy(s_number3_bitmap);

	if ((hour / 10) < 1) {
		draw_blank_to_layer(s_number0_layer, s_number0_bitmap);
	} else {
		draw_number_to_layer(s_number0_layer, s_number0_bitmap, (int) (hour / 10));
	}

	draw_number_to_layer(s_number1_layer, s_number1_bitmap, (int) (hour % 10));
	draw_number_to_layer(s_number2_layer, s_number2_bitmap, (int) (min / 10));
	draw_number_to_layer(s_number3_layer, s_number3_bitmap, (int) (min % 10));
}

static void main_window_load(Window *window) {

	window_set_background_color(window, GColorBlack);

	// Create GBitmap, then set to created BitmapLayer
	s_numbers_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_NUMBERS);

	s_number0_layer = bitmap_layer_create(GRect(16, 28, SPRITE_SIZE, SPRITE_SIZE));
	s_number1_layer = bitmap_layer_create(GRect(80, 28, SPRITE_SIZE, SPRITE_SIZE));
	s_number2_layer = bitmap_layer_create(GRect(16, 92, SPRITE_SIZE, SPRITE_SIZE));
	s_number3_layer = bitmap_layer_create(GRect(80, 92, SPRITE_SIZE, SPRITE_SIZE));

	layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(s_number0_layer));
	layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(s_number1_layer));
	layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(s_number2_layer));
	layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(s_number3_layer));

	update_time();
}

static void main_window_unload(Window *window) {
	gbitmap_destroy(s_numbers_bitmap);

	gbitmap_destroy(s_number0_bitmap);
	gbitmap_destroy(s_number1_bitmap);
	gbitmap_destroy(s_number2_bitmap);
	gbitmap_destroy(s_number3_bitmap);

	bitmap_layer_destroy(s_number0_layer);
	bitmap_layer_destroy(s_number1_layer);
	bitmap_layer_destroy(s_number2_layer);
	bitmap_layer_destroy(s_number3_layer);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
	update_time();
}

static void init() {
	// Create main Window element and assign to pointer
	s_main_window = window_create();

	// Set handlers to manage the elements inside the Window
	window_set_window_handlers(s_main_window, (WindowHandlers) {
		.load = main_window_load,
		.unload = main_window_unload
	});

	// Show the Window on the watch, with animated=true
	window_stack_push(s_main_window, true);

	// Register with TickTimerService
	tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
}

static void deinit() {
	// Destroy Window
	window_destroy(s_main_window);
}

int main(void) {
	init();
	app_event_loop();
	deinit();
}