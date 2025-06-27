#include "elven/registry.h"
#include "elven/client_state.h"
#include "wlr_layer_shell_unstable_v1.h" // From protocols dir
#include <string.h>
#include <wayland-client.h>

static void registry_global(void *data, struct wl_registry *wl_registry,
                            uint32_t name, const char *interface,
                            uint32_t version) {
  struct Bar *state = data;
  if (strcmp(interface, wl_shm_interface.name) == 0) {
    state->wl_shm =
      wl_registry_bind(wl_registry, name, &wl_shm_interface, version);
  } else if (strcmp(interface, wl_compositor_interface.name) == 0) {
    state->wl_compositor =
      wl_registry_bind(wl_registry, name, &wl_compositor_interface, version);
  } else if (strcmp(interface, wl_output_interface.name) == 0) {
    state->wl_output =
      // wl_registry_bind(wl_registry, name, &wl_output_interface, version);
      // //Give some erro of opcode 4
      wl_registry_bind(wl_registry, name, &wl_output_interface, 4);
  } else if (strcmp(interface, zwlr_layer_shell_v1_interface.name) == 0) {
    state->zwlr_layer_shell = wl_registry_bind(
      wl_registry, name, &zwlr_layer_shell_v1_interface, version);
  } else if (strcmp(interface, zwlr_layer_surface_v1_interface.name) == 0) {
    state->zwlr_layer_surface = wl_registry_bind(
      wl_registry, name, &zwlr_layer_surface_v1_interface, version);
  }
}

static void registry_global_remove(void *data, struct wl_registry *wl_registry,
                                   uint32_t name) {
  wl_registry_destroy(wl_registry);
}

static const struct wl_registry_listener wl_registry_listener = {
  .global = registry_global, .global_remove = registry_global_remove};

void init_registry(struct Bar *state) {
  state->wl_display = wl_display_connect(NULL);
  state->wl_registry = wl_display_get_registry(state->wl_display);
  wl_registry_add_listener(state->wl_registry, &wl_registry_listener, state);
  wl_display_roundtrip(state->wl_display);
}
