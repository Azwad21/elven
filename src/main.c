#include "elven.h"
#include "elven/client_state.h"
#include "elven/output_context.h"
#include "elven/registry.h"
#include "elven/render_context.h"
#include "elven/wl_output.h"
#include "elven/zwlr_layer_shell.h"
#include <cairo.h>
#include <poll.h>
#include <stdio.h>
#include <sys/mman.h>
#include <wayland-client.h>
#include <wayland-util.h>

int main(void) {
  struct Bar bar = {.monitor_height = 10, .monitor_width = 640};
  struct RenderContext render_context = {0};
  struct OutputContext output_context = {0};

  struct wl_list test_list;

  struct element {
    int a;
    struct wl_list link;
  };

  struct element a, b, c;
  a.a = 1;
  b.a = 2;
  c.a = 3;

  wl_list_init(&test_list);
  wl_list_insert(&test_list, &a.link);
  wl_list_insert(&test_list, &b.link);
  wl_list_insert(&test_list, &c.link);

  struct element *e;
  wl_list_for_each(e, &test_list, link) {
    printf("Hello world: %d\n", e->a);
  };

  init_registry(&bar);
  init_wl_output(&bar, &output_context);
  init_zwlr(&bar, &render_context);
  wl_surface_commit(bar.wl_surface);

  // Test Cairo
  int stride = bar.monitor_width * 4;
  int size = bar.monitor_height * stride;

  cairo_surface_t *surface;
  cairo_t *cr;
  surface = cairo_image_surface_create_for_data(
    (unsigned char *)render_context.data, CAIRO_FORMAT_ARGB32,
    bar.monitor_width, 10, stride);
  cr = cairo_create(surface);
  cairo_set_source_rgb(cr, 1.0, 0.0, 0.0);
  cairo_paint_with_alpha(cr, 1.0);
  wl_surface_attach(bar.wl_surface, render_context.wl_buffer, 0, 0);
  wl_surface_damage(bar.wl_surface, 0, 0, bar.monitor_width, 10);
  wl_surface_commit(bar.wl_surface);
  // -- End --

  struct pollfd fds[1];
  fds[0].fd = wl_display_get_fd(bar.wl_display);
  fds[0].events = POLLIN;

  while (1) {
    wl_display_flush(bar.wl_display);

    int ret = poll(fds, 1, -1);
    if (ret > 0) {
      if (fds[0].revents & POLLIN) {
        wl_display_dispatch(bar.wl_display);
      }
    }
  }

  munmap(render_context.data, size);
  return 0;
}
