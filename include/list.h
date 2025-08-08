#include <stdint.h>
#include <stdbool.h>

#ifndef LIST_H
#define LIST_H

typedef struct {
  int capacity;
  int length;
  void **items;
} elv_list;

elv_list *list_create();

void list_add(elv_list *list, void *item);

void list_remove(elv_list *list, void *item);
void list_destroy(elv_list *list);

#endif
