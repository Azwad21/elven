#include "elven/bar.h"
#include "wlr_layer_shell_unstable_v1.h" // From protocols dir
#include <stdbool.h>
#include <string.h>
#include <wayland-client-protocol.h>
#include <wayland-client.h>

static void handle_registry_global(void *data, struct wl_registry *wl_registry,
  uint32_t name, const char *interface, uint32_t version) {
  struct elv_bar *bar = data;
  if (strcmp(interface, wl_shm_interface.name) == 0) {
    bar->shm = wl_registry_bind(wl_registry, name, &wl_shm_interface, version);
  } else if (strcmp(interface, wl_compositor_interface.name) == 0) {
    bar->compositor =
      wl_registry_bind(wl_registry, name, &wl_compositor_interface, version);
  } else if (strcmp(interface, wl_output_interface.name) == 0) {
    bar->output = wl_registry_bind(wl_registry, name, &wl_output_interface, 4);
  } else if (strcmp(interface, zwlr_layer_shell_v1_interface.name) == 0) {
    bar->zwlr_layer_shell = wl_registry_bind(
      wl_registry, name, &zwlr_layer_shell_v1_interface, version);
  } else if (strcmp(interface, zwlr_layer_surface_v1_interface.name) == 0) {
    bar->zwlr_layer_surface = wl_registry_bind(
      wl_registry, name, &zwlr_layer_surface_v1_interface, version);
  }
}

static void handle_registry_global_remove(
  void *data, struct wl_registry *wl_registry, uint32_t name) {
  wl_registry_destroy(wl_registry);
}

static const struct wl_registry_listener registry_listener = {
  .global = handle_registry_global,
  .global_remove = handle_registry_global_remove,
};

bool bar_setup(struct elv_bar *bar) {
  bar->display = wl_display_connect(NULL);
  bar->registry = wl_display_get_registry(bar->display);
  wl_registry_add_listener(bar->registry, &registry_listener, &bar);
  return false;
};

void bar_run(struct elv_bar *bar) {};
