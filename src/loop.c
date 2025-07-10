#include <elven/loop.h>
#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

struct loop *loop_create(void) {
  struct loop *loop = calloc(1, sizeof *loop);
  loop->timers = calloc(1, sizeof(struct timers));
  loop->timers->length = 0;
  loop->timers->capacity = 10;
  loop->timers->items =
    malloc(sizeof(struct loop_timer *) * loop->timers->capacity);

  return loop;
}

void loop_add_timer(
  struct loop *loop, int ms, void *data, void (*callback)(void *data)) {
  struct loop_timer *timer = malloc(sizeof *timer);

  if (loop->timers->length == loop->timers->capacity) {
    loop->timers->capacity *= 2;
    loop->timers->items = realloc(loop->timers->items,
      sizeof(struct loop_timer *) * loop->timers->capacity);
  }

  struct timespec expiry;
  clock_gettime(CLOCK_MONOTONIC, &expiry);
  expiry.tv_sec += ms / 1000;
  expiry.tv_nsec += ms * 1000000;

  timer->expiry = expiry;
  timer->data = data;
  timer->callback = callback;

  loop->timers->items[loop->timers->length++] = timer;
}

void loop_poll(struct loop *loop) {
  if (loop->timers) {
    for (int i = 0; i < loop->timers->length; i++) {
      struct loop_timer *timer = loop->timers->items[i];

      struct timespec now;
      clock_gettime(CLOCK_MONOTONIC, &now);

      bool expiry = timer->expiry.tv_sec < now.tv_sec ||
                    (timer->expiry.tv_sec == now.tv_sec &&
                      timer->expiry.tv_nsec < now.tv_nsec);
      if (expiry) {
        timer->callback(timer->data);
        loop_remove_timer(loop, timer);
      }
    }
  }
}

bool loop_remove_timer(struct loop *loop, struct loop_timer *timer) {
  for (int i = 0; i < loop->timers->length; i++) {
    struct loop_timer *tmp = loop->timers->items[i];
    if (tmp == timer) {
      loop->timers->length--;
      memmove(&loop->timers->items[i], &loop->timers->items[i + 1],
        sizeof(struct loop_timer *) * (loop->timers->length - i));
      free(timer);
      return true;
    }
  }
  return false;
}
