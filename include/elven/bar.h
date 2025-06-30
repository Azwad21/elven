#include <stdbool.h>

#ifndef BAR_H
#define BAR_H

struct elv_bar {
  struct wl_display *display;
  struct wl_registry *registry;
  struct wl_shm *shm;
  struct wl_compositor *compositor;
  struct wl_output *output;
  struct wl_surface *surface;
  struct zwlr_layer_shell_v1 *zwlr_layer_shell;
  struct zwlr_layer_surface_v1 *zwlr_layer_surface;
  struct wl_array *outputs;
  int monitor_width;
  int monitor_height;
};

void bar_run(struct elv_bar *);
bool bar_setup(struct elv_bar *);

#endif
