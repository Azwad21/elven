#include "bar.h"
#include "config.h"
#include "loop.h"
#include "pool-buffer.h"
#include "render.h"
#include "wlr_layer_shell_unstable_v1.h"
#include <assert.h>
#include <errno.h>
#include <poll.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wayland-client-protocol.h>
#include <wayland-client.h>
#include <wayland-util.h>

static void handle_layer_surface_configure(void *data,
  struct zwlr_layer_surface_v1 *zwlr_layer_surface_v1, uint32_t serial,
  uint32_t width, uint32_t height) {
  struct elv_output *output = data;
  output->width = width;
  output->height = height;
  printf("Width: %d\n", width);
  printf("Height: %d\n", height);

  zwlr_layer_surface_v1_ack_configure(zwlr_layer_surface_v1, serial);

  int pool_buff_len =
    sizeof(output->pool_buffers) / sizeof(struct pool_buffer *);

  for (int i = 0; i < pool_buff_len; i++) {
    output->pool_buffers[i] = get_buffer(output->bar->shm, width, height);
  }
  output->current_buffer = output->pool_buffers[0];

  wl_surface_attach(output->surface, output->current_buffer->buffer, 0, 0);
  wl_surface_commit(output->surface);
  render_frame(output);
}

static void handle_layer_surface_closed(
  void *data, struct zwlr_layer_surface_v1 *zwlr_layer_surface_v1) {
}

static const struct zwlr_layer_surface_v1_listener layer_surface_listener = {
  .configure = handle_layer_surface_configure,
  .closed = handle_layer_surface_closed,
};

static void add_layer_frame(struct elv_output *output) {
  printf("Reached add_layer_frame\n");
  struct elv_bar_config *config = output->bar->config;
  output->layer_surface = zwlr_layer_shell_v1_get_layer_surface(
    output->bar->layer_shell, output->surface, output->output,
    ZWLR_LAYER_SHELL_V1_LAYER_BOTTOM, "elven_bar");
  zwlr_layer_surface_v1_add_listener(
    output->layer_surface, &layer_surface_listener, output);
  zwlr_layer_surface_v1_set_size(output->layer_surface, 0, config->bar_height);
  zwlr_layer_surface_v1_set_anchor(output->layer_surface,
    ZWLR_LAYER_SURFACE_V1_ANCHOR_TOP | ZWLR_LAYER_SURFACE_V1_ANCHOR_LEFT |
      ZWLR_LAYER_SURFACE_V1_ANCHOR_RIGHT);
  zwlr_layer_surface_v1_set_margin(output->layer_surface, 0, 0, 0, 0);
  zwlr_layer_surface_v1_set_exclusive_zone(
    output->layer_surface, config->bar_height);
  wl_surface_commit(output->surface);
}

static void handle_output_mode(void *data, struct wl_output *wl_output,
  uint32_t flags, int32_t width, int32_t height, int32_t refresh) {
}

static void handle_output_done(void *data, struct wl_output *wl_output) {
  struct elv_output *output = data;
  output->surface = wl_compositor_create_surface(output->bar->compositor);
  add_layer_frame(output);
}

static void handle_output_scale(
  void *data, struct wl_output *wl_output, int32_t factor) {
  (void)data;
  (void)wl_output;
  (void)factor;
}

static void handle_output_geometry(void *data, struct wl_output *wl_output,
  int32_t x, int32_t y, int32_t physical_width, int32_t physical_height,
  int32_t subpixel, const char *make, const char *model, int32_t transform) {
  (void)data;
  (void)wl_output;
  (void)x;
  (void)y;
  (void)physical_width;
  (void)physical_height;
  (void)subpixel;
  (void)make;
  (void)model;
}

static void handle_output_description(
  void *data, struct wl_output *wl_output, const char *description) {
}
static void handle_output_name(
  void *data, struct wl_output *wl_output, const char *name) {
  struct elv_output *elv_output = data;
  elv_output->name = name;
}

static const struct wl_output_listener output_listener = {
  .mode = handle_output_mode,
  .done = handle_output_done,
  .scale = handle_output_scale,
  .geometry = handle_output_geometry,
  .name = handle_output_name,
  .description = handle_output_description,
};

static void handle_registry_global(void *data, struct wl_registry *wl_registry,
  uint32_t name, const char *interface, uint32_t version) {
  struct elv_bar *bar = data;
  if (strcmp(interface, wl_shm_interface.name) == 0) {
    bar->shm = wl_registry_bind(wl_registry, name, &wl_shm_interface, version);
  } else if (strcmp(interface, wl_compositor_interface.name) == 0) {
    bar->compositor =
      wl_registry_bind(wl_registry, name, &wl_compositor_interface, version);
  } else if (strcmp(interface, wl_output_interface.name) == 0) {
    struct elv_output *elv_output = malloc(sizeof *elv_output);
    elv_output->output =
      wl_registry_bind(wl_registry, name, &wl_output_interface, 4);
    elv_output->bar = bar;
    wl_output_add_listener(elv_output->output, &output_listener, elv_output);
    wl_list_insert(&bar->outputs, &elv_output->link);
  } else if (strcmp(interface, zwlr_layer_shell_v1_interface.name) == 0) {
    bar->layer_shell = wl_registry_bind(
      wl_registry, name, &zwlr_layer_shell_v1_interface, version);
  }
}

static void handle_registry_global_remove(
  void *data, struct wl_registry *wl_registry, uint32_t name) {
  // wl_registry_destroy(wl_registry);
}

static const struct wl_registry_listener registry_listener = {
  .global = handle_registry_global,
  .global_remove = handle_registry_global_remove,
};

static void display(int fd, short flags, void *data) {
  printf("Display being called\n");
  struct elv_bar *bar = data;
  // wl_display_flush(bar->display);
  //
  // if (wl_display_prepare_read(bar->display) == 0) {
  //   wl_display_read_events(bar->display);
  //   wl_display_dispatch_pending(bar->display);
  // } else {
  //   wl_display_dispatch_pending(bar->display);
  // }
  wl_display_dispatch(bar->display);
}

bool bar_setup(struct elv_bar *bar) {
  bar->display = wl_display_connect(NULL);
  bar->display_fd = wl_display_get_fd(bar->display);
  assert(bar->display != NULL);

  if (!bar->display) {
    fprintf(stderr, "Failed to create wayland display\n");
    return false;
  }

  wl_list_init(&bar->outputs);

  bar->registry = wl_display_get_registry(bar->display);
  wl_registry_add_listener(bar->registry, &registry_listener, bar);
  wl_display_roundtrip(bar->display);

  // Config setup
  bar->config = init_config();

  // Event loop setup
  bar->eventloop = loop_create();
  return true;
};

void bar_run(struct elv_bar *bar) {
  // printf("Printing fd capacity: %d\n", bar->eventloop->fd_capacity);
  loop_add_fd(bar->eventloop, bar->display_fd, POLLIN, bar, display);

  // struct pollfd pfds[1];
  // pfds[0].fd = wl_display_get_fd(bar->display);
  // pfds[0].events = POLLIN;

  while (bar->running) {
    // printf("Loop running\n");
    // wl_display_flush(bar->display);
    //
    // if (wl_display_prepare_read(bar->display) == 0) {
    //
    //   if (poll(pfds, 1, 15) > 0) {
    //     wl_display_read_events(bar->display);
    //   }
    //
    //   wl_display_dispatch_pending(bar->display);
    //
    // } else {
    //   wl_display_dispatch_pending(bar->display);
    // }

    if (wl_display_flush(bar->display) == -1 && errno == EAGAIN) {
      break;
    }
    loop_poll(bar->eventloop);
  }
};

static void output_destroy(struct elv_bar *bar) {
  struct elv_output *tmp;
  wl_list_for_each(tmp, &bar->outputs, link) {
    zwlr_layer_surface_v1_destroy(tmp->layer_surface);
    wl_surface_destroy(tmp->surface);
    wl_output_destroy(tmp->output);
    // destroy_buffer(tmp->pool_buffers);
    wl_list_remove(&tmp->link);
    free(tmp);
    tmp = NULL;
  }
}

void bar_destroy(struct elv_bar *bar) {
  struct elv_output *tmp;
  wl_list_for_each(tmp, &bar->outputs, link) {
    wl_output_destroy(tmp->output);
    // destroy_buffer(tmp->pool_buffers);
    wl_list_remove(&tmp->link);
    free(tmp);
    tmp = NULL;
  }
  wl_display_disconnect(bar->display);
}

// bar_destroy: also free bar->outputs
