#include <wayland-client-protocol.h>
#include <stdint.h>

#ifndef POOL_BUFFER_H
#define POOL_BUFFER_H

struct pool_buffer {
  struct wl_buffer *buffer;
};

struct pool_buffer *get_buffer(struct wl_shm *shm, uint32_t width, uint32_t height);
void destroy_buffer(struct pool_buffer *render_ctx);

#endif
