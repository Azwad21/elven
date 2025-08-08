#include "color.h"

#ifndef _ELVEN_CONFIG_H
#define _ELVEN_CONFIG_H

struct elv_bar_background {
  struct elv_color color;
};

struct elv_bar_config {
  int bar_height;
};

struct elv_bar_config *init_config();

#endif
