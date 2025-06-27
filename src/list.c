#include "elven/list.h"
#include <stdlib.h>
#include <wayland-util.h>

struct wl_list *list_create() {
  struct wl_list *list = (struct wl_list *)malloc(sizeof(struct wl_list));
  wl_list_init(list);
  return list;
}

void list_add(struct wl_list *list, struct wl_list *link) {
  wl_list_insert(list, link);
}
