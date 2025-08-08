#include "bar.h"
#include "bar.h"
#include <cairo.h>

#ifndef _ELVEN_RENDER_H
#define _ELVEN_RENDER_H

struct render_ctx {
  cairo_t *cairo;
  cairo_surface_t *surface;

  struct elv_output *output;
};

void render_frame(struct elv_output *output);

#endif
