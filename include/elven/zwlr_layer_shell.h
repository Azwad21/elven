#include "client_state.h"
#include "output_context.h"
#include "render_context.h"

#ifndef ZWLR_LAYER_SHELL
#define ZWLR_LAYER_SHELL

void init_zwlr(struct Bar *state, struct RenderContext *render_context);

#endif
