#include "elven/render.h"
#include "elven/bar.h"
#include "elven/pool-buffer.h"
#include <cairo.h>
#include <wayland-client-protocol.h>
#include <wayland-client.h>
#include <wayland-util.h>

void render_frame(struct elv_output *output) {
  cairo_surface_t *surface;
  cairo_t *cr;

  surface = cairo_image_surface_create_for_data(output->pool_buffer->pool_data,
    CAIRO_FORMAT_ARGB32, output->width, output->height,
    output->pool_buffer->stride);

  cr = cairo_create(surface);
  cairo_set_source_rgb(cr, 1.0, 0.0, 0.0);
  cairo_paint_with_alpha(cr, 1.0);
  cairo_surface_flush(surface);

  wl_surface_damage(output->surface, 0, 0, output->width, output->height);
  wl_surface_attach(output->surface, output->pool_buffer->buffer, 0, 0);
  wl_surface_commit(output->surface);
}
