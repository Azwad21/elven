#include "client_state.h"
#include "output_context.h"

#ifndef WL_OUTPUT_H
#define WL_OUTPUT_H

void init_wl_output(struct ClientState *state,
                    struct OutputContext *output_context);

#endif
