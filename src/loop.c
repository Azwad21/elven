#include "loop.h"
#include "list.h"
#include <errno.h>
#include <limits.h>
#include <poll.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/poll.h>
#include <time.h>

struct loop *loop_create(void) {
  struct loop *loop = calloc(1, sizeof *loop);

  loop->timers = list_create();
  loop->fd_events = list_create();
  loop->fd_length = 0;
  loop->fd_capacity = 10;

  loop->fds = malloc(sizeof(struct pollfd) * loop->fd_capacity);

  return loop;
}

void loop_add_fd(struct loop *loop, int fd, short flags, void *data,
  void (*callback)(int fd, short flags, void *data)) {

  struct loop_fd_event *event = calloc(1, sizeof *event);

  event->callback = callback;
  event->data = data;
  list_add(loop->fd_events, event);

  struct pollfd pfd = {
    .fd = fd,
    .events = flags,
    .revents = 0,
  };

  if (loop->fd_length == loop->fd_capacity) {
    loop->fd_capacity += 10;
    struct pollfd *tmp =
      realloc(loop->fds, sizeof(struct pollfd) * loop->fd_capacity);
    if (!tmp) {
      fprintf(stderr, "Failed to reallocate fds: loop_add_fd\n");
      exit(EXIT_FAILURE);
    }

    loop->fds = tmp;
  }

  loop->fds[loop->fd_length++] = pfd;
}

void loop_add_timer(
  struct loop *loop, int ms, void *data, void (*callback)(void *data)) {
  struct loop_timer *timer = malloc(sizeof *timer);

  struct timespec expiry;
  clock_gettime(CLOCK_MONOTONIC, &expiry);
  expiry.tv_sec += ms / 1000;

  long int nsec = (ms % 1000) * 1000000;

  if (expiry.tv_nsec + nsec >= 1000000000) {
    expiry.tv_sec++;
    nsec -= 1000000000;
  }

  expiry.tv_nsec += nsec;

  timer->expiry = expiry;
  timer->data = data;
  timer->callback = callback;

  list_add(loop->timers, timer);
}

void loop_poll(struct loop *loop) {
  int ms = INT_MAX;

  if (loop->timers->length) {
    struct timespec now;
    clock_gettime(CLOCK_MONOTONIC, &now);
    for (int i = 0; i < loop->timers->length; i++) {
      struct loop_timer *timer = loop->timers->items[i];
      int timer_ms = (timer->expiry.tv_sec - now.tv_sec) * 1000;
      timer_ms += (timer->expiry.tv_nsec - now.tv_nsec) / 1000000;

      if (timer_ms < ms) {
        ms = timer_ms;
      }
    }
  }
  if (ms < 0) {
    ms = 0;
  }

  // Dispatch pollfd
  if (poll(loop->fds, loop->fd_length, INT_MAX) > 0) {
    for (int i = 0; i < loop->fd_length; i++) {
      struct pollfd pfd = loop->fds[i];
      struct loop_fd_event *event = loop->fd_events->items[i];

      unsigned events = pfd.events | POLLHUP | POLLERR;

      if (pfd.revents & events) {
        event->callback(pfd.fd, pfd.events, event->data);
      }
    }
  }

  // Dispatch timers
  if (loop->timers && loop->timers->length) {
    struct timespec now;
    clock_gettime(CLOCK_MONOTONIC, &now);
    for (int i = 0; i < loop->timers->length; i++) {
      struct loop_timer *timer = loop->timers->items[i];

      bool expiry = timer->expiry.tv_sec < now.tv_sec ||
                    (timer->expiry.tv_sec == now.tv_sec &&
                      timer->expiry.tv_nsec < now.tv_nsec);
      if (expiry) {
        timer->callback(timer->data);
        loop_remove_timer(loop, timer);
        i--;
      }
    }
  }
}

bool loop_remove_timer(struct loop *loop, struct loop_timer *timer) {
  list_remove(loop->timers, timer);
  return false;
}

bool loop_destroy(struct loop *loop) {
  list_destroy(loop->timers);
  free(loop);
  loop = NULL;
}
