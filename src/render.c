#include "render.h"
#include "bar.h"
#include "pool-buffer.h"
#include <cairo.h>
#include <math.h>
#include <pango/pangocairo.h>
#include <stdbool.h>
#include <stdio.h>
#include <time.h>
#include <wayland-client-protocol.h>
#include <wayland-client.h>
#include <wayland-util.h>

static void recalculate_current_buffer(struct elv_output *output) {
  for (int i = 0;
    i < sizeof(output->pool_buffers) / sizeof(struct pool_buffer *); i++) {
    if (output->pool_buffers[i]->busy == false) {
      output->current_buffer = output->pool_buffers[i];
      printf("Buffer at index: %d setted\n", i);
      break;
    }
  }
}

static const struct wl_callback_listener wl_surface_frame_listener;

static void draw_1(void *data, struct wl_callback *cb, uint32_t callback_data) {
  wl_callback_destroy(cb);

  struct elv_output *output = data;
  cb = wl_surface_frame(output->surface);
  wl_callback_add_listener(cb, &wl_surface_frame_listener, output);

  cairo_surface_t *surface;
  cairo_t *cr;
  surface = cairo_image_surface_create_for_data(
    output->current_buffer->pool_data, CAIRO_FORMAT_ARGB32, output->width,
    output->height, output->current_buffer->stride);

  cr = cairo_create(surface);

  char name[9];

  time_t now = time(NULL);
  struct tm *tm_info = localtime(&now);
  strftime(name, sizeof(name), "%H:%M:%S", tm_info);

  int b_width = output->width / 2.5;
  int start_x = (output->width - b_width) / 2;

  double x1 = 9.9;
  int y1 = (x1 * output->height) / 10;

  int x2 = 10;
  int y2 = output->height;

  int x3 = 30;
  int y3 = output->height;

  cairo_move_to(cr, start_x, 0);
  cairo_curve_to(cr, start_x + x1, y1, start_x + x2, y2, start_x + x3, y3);
  cairo_line_to(cr, start_x + x3, 0);

  cairo_move_to(cr, start_x + b_width, 0);
  cairo_curve_to(cr, start_x + b_width - x1, y1, start_x + b_width - x2, y2,
    start_x + b_width - x3, y3);
  cairo_line_to(cr, start_x + b_width - x3, 0);

  cairo_rectangle(cr, start_x + x3, 0, b_width - x3 - x3, output->height);

  cairo_set_source_rgb(cr, 1, 1, 1);
  cairo_fill(cr);

  cairo_text_extents_t te;
  cairo_set_source_rgb(cr, 0, 0, 0);
  cairo_select_font_face(
    cr, "Adwaita Sans", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
  cairo_set_font_size(cr, 15);
  cairo_text_extents(cr, name, &te);
  cairo_move_to(cr, (output->width / 2.0) - (te.width / 2.0 + te.x_bearing),
    (output->height / 2.0) - (te.height / 2.0 + te.y_bearing));

  cairo_show_text(cr, name);
  output->current_buffer->busy = true;
  wl_surface_damage(output->surface, 0, 0, output->width, output->height);
  wl_surface_attach(output->surface, output->current_buffer->buffer, 0, 0);
  wl_surface_commit(output->surface);
  recalculate_current_buffer(output);
}

static const struct wl_callback_listener wl_surface_frame_listener = {
  .done = draw_1,
};

void render_frame(struct elv_output *output) {
  wl_surface_commit(output->surface);

  struct wl_callback *cb = wl_surface_frame(output->surface);
  wl_callback_add_listener(cb, &wl_surface_frame_listener, output);
}
