#include "config.h"
#include <stdlib.h>

struct elv_bar_config *init_config() {
  struct elv_bar_config *config = malloc(sizeof *config);
  config->bar_height = 20;
  return config;
}
