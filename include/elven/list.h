#include <wayland-util.h>

#ifndef LIST_H
#define LIST_H

struct wl_list* list_create();
void list_add(struct wl_list *list, struct wl_list *link);
void list_remove(struct wl_list *list);
void list_destroy();

#endif
