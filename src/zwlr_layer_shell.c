#include "elven/zwlr_layer_shell.h"
#include "elven/buffer.h"
#include "elven/client_state.h"
#include "elven/render_context.h"
#include "wlr_layer_shell_unstable_v1.h" // From protocols dir
#include <cairo.h>
#include <stdlib.h>
#include <wayland-client-protocol.h>
struct ZwlrUserData {
  struct Bar *state;
  struct RenderContext *render_context;
};

static void zwlr_layer_surface_configure(
  void *data, struct zwlr_layer_surface_v1 *zwlr_layer_surface_v1,
  uint32_t serial, uint32_t width, uint32_t height) {
  zwlr_layer_surface_v1_ack_configure(zwlr_layer_surface_v1, serial);

  struct ZwlrUserData *t = data;
  create_buffer(t->state, t->render_context);
  wl_surface_attach(t->state->wl_surface, t->render_context->wl_buffer, 0, 0);
  wl_surface_commit(t->state->wl_surface);

  // int stride = t->state->monitor_width * 4;
  // int size = t->state->monitor_height * stride;
  // cairo_surface_t *surface;
  // cairo_t *cr;
  // surface = cairo_image_surface_create_for_data(
  //     (unsigned char *)t->render_context->data, CAIRO_FORMAT_ARGB32,
  //     t->state->monitor_width, 10, stride);
  // cr = cairo_create(surface);
  // cairo_set_source_rgb(cr, 1.0, 0.0, 0.0);
  // cairo_paint_with_alpha(cr, 1.0);
  // wl_surface_attach(t->state->wl_surface, t->render_context->wl_buffer, 0,
  // 0); wl_surface_damage(t->state->wl_surface, 0, 0, t->state->monitor_width,
  //                   t->state->monitor_height);
  // wl_surface_commit(t->state->wl_surface);
}

static void
zwlr_layer_surface_closed(void *data,
                          struct zwlr_layer_surface_v1 *zwlr_layer_surface_v1) {
  free(data);
}

static const struct zwlr_layer_surface_v1_listener
  zwlr_layer_surface_v1_listener = {
    .configure = zwlr_layer_surface_configure,
    .closed = zwlr_layer_surface_closed,
};

void init_zwlr(struct Bar *state, struct RenderContext *render_context) {

  struct ZwlrUserData *zwlr_user_data = malloc(sizeof(struct ZwlrUserData));
  zwlr_user_data->state = state;
  zwlr_user_data->render_context = render_context;

  state->wl_surface = wl_compositor_create_surface(state->wl_compositor);

  state->zwlr_layer_surface = zwlr_layer_shell_v1_get_layer_surface(
    state->zwlr_layer_shell, state->wl_surface, state->wl_output,
    ZWLR_LAYER_SHELL_V1_LAYER_BOTTOM, "elven_status_bar");

  zwlr_layer_surface_v1_add_listener(
    state->zwlr_layer_surface, &zwlr_layer_surface_v1_listener, zwlr_user_data);
  zwlr_layer_surface_v1_set_anchor(state->zwlr_layer_surface,
                                   ZWLR_LAYER_SURFACE_V1_ANCHOR_TOP |
                                     ZWLR_LAYER_SURFACE_V1_ANCHOR_LEFT |
                                     ZWLR_LAYER_SURFACE_V1_ANCHOR_RIGHT);
  zwlr_layer_surface_v1_set_size(state->zwlr_layer_surface, 0, 10);
  zwlr_layer_surface_v1_set_margin(state->zwlr_layer_surface, 0, 0, 0, 0);
  zwlr_layer_surface_v1_set_exclusive_zone(state->zwlr_layer_surface, 10);
}
