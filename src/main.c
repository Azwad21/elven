#include "elven.h"
#include "elven/client_state.h"
#include "elven/output_context.h"
#include "elven/registry.h"
#include "elven/render_context.h"
#include "elven/wl_output.h"
#include "elven/zwlr_layer_shell.h"
#include <cairo.h>
#include <poll.h>
#include <sys/mman.h>
#include <wayland-client.h>
#include <wayland-util.h>

int main(void) {
  struct ClientState state = {.monitor_height = 10, .monitor_width = 640};
  struct RenderContext render_context = {0};
  struct OutputContext output_context = {0};

  struct wl_array array = {0};
  wl_array_init(&array);

  init_registry(&state);
  init_wl_output(&state, &output_context);
  init_zwlr(&state, &render_context);
  wl_surface_commit(state.wl_surface);

  // Test Cairo
  int stride = state.monitor_width * 4;
  int size = state.monitor_height * stride;

  cairo_surface_t *surface;
  cairo_t *cr;
  surface = cairo_image_surface_create_for_data(
    (unsigned char *)render_context.data,
    CAIRO_FORMAT_ARGB32,
    state.monitor_width,
    10,
    stride
  );
  cr = cairo_create(surface);
  cairo_set_source_rgb(cr, 1.0, 0.0, 0.0);
  cairo_paint_with_alpha(cr, 1.0);
  wl_surface_attach(state.wl_surface, render_context.wl_buffer, 0, 0);
  wl_surface_damage(state.wl_surface, 0, 0, state.monitor_width, 10);
  wl_surface_commit(state.wl_surface);
  // -- End --

  struct pollfd fds[1];
  fds[0].fd = wl_display_get_fd(state.wl_display);
  fds[0].events = POLLIN;

  while (1) {
    wl_display_flush(state.wl_display);

    int ret = poll(fds, 1, -1);
    if (ret > 0) {
      if (fds[0].revents & POLLIN) {
        wl_display_dispatch(state.wl_display);
      }
    }
  }

  munmap(render_context.data, size);
  return 0;
}
