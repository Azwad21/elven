#include "client_state.h"
#include "render_context.h"
#include <stddef.h>
#include <wayland-client-protocol.h>
#ifndef INIT_HEADERS
#define INIT_HEADERS

int shm_get_fd(size_t size);
void create_buffer(struct Bar *state,
                   struct RenderContext *render_context);

#endif
