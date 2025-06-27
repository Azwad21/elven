#include "wlr_layer_shell_unstable_v1.h"
#include <stdio.h>
#include <string.h>
#include <wayland-client.h>

static void registry_global(void *data, struct wl_registry *wl_registry,
                            uint32_t name, const char *interface,
                            uint32_t version) {
  printf("%s\n", interface);
}

static void registry_global_remove(void *data, struct wl_registry *wl_registry,
                                   uint32_t name) {
  wl_registry_destroy(wl_registry);
}

static const struct wl_registry_listener wl_registry_listener = {
  .global = registry_global, .global_remove = registry_global_remove};

void init_registry() {
  struct wl_display *wl_display = wl_display_connect(NULL);
  struct wl_registry *wl_registry = wl_display_get_registry(wl_display);
  wl_registry_add_listener(wl_registry, &wl_registry_listener, NULL);
  wl_display_roundtrip(wl_display);
}

int main() {
  init_registry();
  return 0;
}
