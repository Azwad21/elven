#include "elven/wl_output.h"
#include "elven/client_state.h"
#include "elven/output_context.h"
#include <stdio.h>
#include <wayland-client-core.h>
#include <wayland-client-protocol.h>

static void handle_mode(void *data, struct wl_output *wl_output, uint32_t flags,
                        int32_t width, int32_t height, int32_t refresh) {
  struct OutputContext *output_context = data;
  output_context->mointor_height = (int)height;
  output_context->monitor_width = (int)width;
  printf("Width: %d, Height: %d\n", width, height);
}

static void handle_done(void *data, struct wl_output *wl_output) {
  wl_output_destroy(wl_output);
}

static void handle_scale(void *data, struct wl_output *wl_output,
                         int32_t factor) {
  (void)data;
  (void)wl_output;
  (void)factor;
}

static void handle_geometry(void *data, struct wl_output *wl_output, int32_t x,
                            int32_t y, int32_t physical_width,
                            int32_t physical_height, int32_t subpixel,
                            const char *make, const char *model,
                            int32_t transform) {
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

static void handle_name(void *data, struct wl_output *wl_output,
                        const char *name) {
  printf("%s\n", name);
}

static void handle_desc(void *data, struct wl_output *wl_output,
                        const char *description) {
}

static const struct wl_output_listener wl_output_listener = {
  .mode = handle_mode,
  .done = handle_done,
  .scale = handle_scale,
  .geometry = handle_geometry,
  .name = handle_name,
  .description = handle_desc,
};

void init_wl_output(struct ClientState *state,
                    struct OutputContext *output_context) {
  wl_output_add_listener(state->wl_output, &wl_output_listener, output_context);
}
