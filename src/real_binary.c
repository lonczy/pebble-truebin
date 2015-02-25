/*
A "TRUE BINARY" watchface
*/
#include <pebble.h>

static Window *window;
static TextLayer *text_layer;
static Layer *display_layer;
static Layer *date_display_layer;

#define CIRCLE_RADIUS 14
#define CIRCLE_LINE_THICKNESS 2

void draw_cell(GContext* ctx, GPoint center, bool filled) {
  // Each "cell" represents a binary digit or 0 or 1.

  graphics_context_set_fill_color(ctx, GColorWhite);

  graphics_fill_circle(ctx, center, CIRCLE_RADIUS);

  if (!filled) {
    // This is our ghetto way of drawing circles with a line thickness
    // of more than a single pixel.
    graphics_context_set_fill_color(ctx, GColorBlack);

    graphics_fill_circle(ctx, center, CIRCLE_RADIUS - CIRCLE_LINE_THICKNESS);
  }

}

#define CELLS_PER_ROW 4
#define CELLS_PER_COLUMN 4

#define CIRCLE_PADDING 16 - CIRCLE_RADIUS // Number of padding pixels on each side
#define CELL_SIZE (2 * (CIRCLE_RADIUS + CIRCLE_PADDING)) // One "cell" is the square that contains the circle.
#define SIDE_PADDING (144 - (4 * CELL_SIZE))/2
#define TOP_PADDING (168 - (4 * CELL_SIZE))/2

GPoint get_center_point_from_cell_location(unsigned short x, unsigned short y) {
  // Cell location (0,0) is upper left, location (4, 6) is lower right.
  return GPoint(SIDE_PADDING + (CELL_SIZE/2) + (CELL_SIZE * x),
		TOP_PADDING + (CELL_SIZE/2) + (CELL_SIZE * y));
}


/*
static void select_click_handler(ClickRecognizerRef recognizer, void *context) {
  text_layer_set_text(text_layer, "Select");
}

static void up_click_handler(ClickRecognizerRef recognizer, void *context) {
  text_layer_set_text(text_layer, "Up");
}

static void down_click_handler(ClickRecognizerRef recognizer, void *context) {
  text_layer_set_text(text_layer, "Down");
}

static void click_config_provider(void *context) {
  window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
  window_single_click_subscribe(BUTTON_ID_UP, up_click_handler);
  window_single_click_subscribe(BUTTON_ID_DOWN, down_click_handler);
}

static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  text_layer = text_layer_create((GRect) { .origin = { 0, 72 }, .size = { bounds.size.w, 20 } });
  text_layer_set_text(text_layer, "Press a button");
  text_layer_set_text_alignment(text_layer, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(text_layer));
}

static void window_unload(Window *window) {
  text_layer_destroy(text_layer);
}

static void init(void) {
  window = window_create();
  window_set_click_config_provider(window, click_config_provider);
  window_set_window_handlers(window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
  });
  const bool animated = true;
  window_stack_push(window, animated);
}

static void deinit(void) {
  window_destroy(window);
}

int main(void) {
  init();

  APP_LOG(APP_LOG_LEVEL_DEBUG, "Done initializing, pushed window: %p", window);

  app_event_loop();
  deinit();
}

*/

void display_layer_update_callback(Layer *me, GContext* ctx) {

  time_t now = time(NULL);
  struct tm *t = localtime(&now);
  unsigned long long secs = (((t->tm_hour*60)+t->tm_min)*60+t->tm_sec)/86400.0*65536;

  for (int i=0; i<16;i++) {
//void draw_cell(GContext* ctx, GPoint center, bool filled) {
	draw_cell(ctx, get_center_point_from_cell_location(3-(i%4), 3-(i/4)), (secs%2!=0));
//	draw_cell(ctx, get_center_point_from_cell_location(i%4, (i/4)), (secs%2!=0));
	secs=secs/2;
  }  

}

int showdate;

void date_display_layer_update_callback(Layer *me, GContext* ctx) {
//    showdate=3;
//    draw_cell(ctx, get_center_point_from_cell_location(2,2),1);
    
}



void handle_second_tick(struct tm *tick_time, TimeUnits units_changed) {
    if (showdate>0)
        showdate--;
    else
        layer_mark_dirty(display_layer);
}

void tap_handler(AccelAxisType axis, int32_t direction) {
//    if (axis = ACCEL_AXIS_Z) {
//        layer_mark_dirty(date_display_layer);
//    }
}

static void do_init(void) {
  window = window_create();
  window_stack_push(window, true);

  window_set_background_color(window, GColorBlack);

  Layer *root_layer = window_get_root_layer(window);
  GRect frame = layer_get_frame(root_layer);

    showdate=0;
    
  // Init the layer for the display
    display_layer = layer_create(frame);
    date_display_layer = layer_create(frame);
    layer_set_update_proc(display_layer, &display_layer_update_callback);
    layer_set_update_proc(date_display_layer, &date_display_layer_update_callback);
    layer_add_child(root_layer, display_layer);
    layer_add_child(root_layer, date_display_layer);
    
    tick_timer_service_subscribe(SECOND_UNIT, &handle_second_tick);
    accel_tap_service_subscribe(&tap_handler);
}

static void do_deinit(void) {
  layer_destroy(display_layer);
  window_destroy(window);
    accel_tap_service_unsubscribe();
}


int main(void) {
  do_init();
  app_event_loop();
  do_deinit();
}
