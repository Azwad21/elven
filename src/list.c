#include "list.h"
#include "string.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

elv_list *list_create() {
  elv_list *list = malloc(sizeof *list);
  list->capacity = 10;
  list->length = 0;
  list->items = malloc(sizeof(void *) * list->capacity);

  return list;
}

static void list_resize(elv_list *list) {
  if (list->capacity == list->length) {
    list->capacity *= 2;
    list->items = realloc(list->items, sizeof(void *) * list->capacity);
  }
}

void list_add(elv_list *list, void *data) {
  list_resize(list);
  list->items[list->length++] = data;
}

void list_remove(elv_list *list, void *data) {
  for (int i = 0; i < list->length; i++) {
    void *tmp = list->items[i];
    if (tmp == data) {
      list->length--;
      memmove(&list->items[i], &list->items[i + 1],
        sizeof(void *) * (list->length - i));
      free(data);
    }
  }
}

void list_destroy(elv_list *list) {
  for (int i = 0; i < list->length; i++) {
    free(list->items[i]);
  }
  free(list->items);
  free(list);
}
