#include <wayland-client-protocol.h>
#include <stdint.h>

#ifndef POOL_BUFFER_H
#define POOL_BUFFER_H

struct render_context {
  struct wl_buffer *buffer;
};

struct render_context *get_buffer(struct wl_shm *shm, uint32_t width, uint32_t height);
void destroy_buffer(struct render_context *render_ctx);

#endif
