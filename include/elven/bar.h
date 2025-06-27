#ifndef BAR_H
#define BAR_H

struct elv_bar {
  struct wl_display *wl_display;
  struct wl_registry *wl_registry;
  struct wl_shm *wl_shm;
  struct wl_compositor *wl_compositor;
  struct wl_output *wl_output;
  struct wl_surface *wl_surface;
  struct zwlr_layer_shell_v1 *zwlr_layer_shell;
  struct zwlr_layer_surface_v1 *zwlr_layer_surface;
  struct wl_array *outputs;
  int monitor_width;
  int monitor_height;
};

#endif BAR_H
