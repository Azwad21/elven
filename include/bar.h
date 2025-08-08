#include <stdbool.h>
#include <wayland-client-protocol.h>
#include <stdint.h>
#include "config.h"
#include <wayland-util.h>
#include "loop.h"

#ifndef BAR_H
#define BAR_H

struct elv_bar {
  struct wl_display *display;
  struct wl_registry *registry;
  struct wl_shm *shm;
  struct wl_compositor *compositor;
  struct zwlr_layer_shell_v1 *layer_shell;
  struct wl_list outputs; // struct elv_output[]
  struct elv_bar_config *config;
  struct loop *eventloop;
  int display_fd;
  bool running;
};

struct elv_output {
  struct wl_output *output;
  struct elv_bar *bar;
  struct wl_surface *surface;
  struct zwlr_layer_surface_v1 *layer_surface;
  uint32_t width, height;
  uint32_t output_width, output_height;
  struct wl_list link;
  const char *name;

  struct pool_buffer *current_buffer;
  struct pool_buffer *pool_buffers[3];
};

void bar_run(struct elv_bar *bar);
bool bar_setup(struct elv_bar *bar);
void bar_destroy(struct elv_bar *bar);

#endif
