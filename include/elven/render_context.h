#include <stdint.h>

#ifndef RENDER_CONTEXT_H
#define RENDER_CONTEXT_H

struct RenderContext {
  struct wl_buffer *wl_buffer;
  uint8_t *data;
};

#endif
