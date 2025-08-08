#include "list.h"
#include <stdbool.h>
#include <time.h>
#include <poll.h>

#ifndef _ELVEN_LOOP_H
#define _ELVEN_LOOP_H

#define INT_MAX_TIMER 1000*30

struct loop_fd_event {
  void (*callback)(int fd, short flags, void *data);
  void *data;
};

struct loop_timer {
  void (*callback)(void *data);
  void *data;
  struct timespec expiry;
};

struct loop {
  struct pollfd *fds;
  int fd_length;
  int fd_capacity;

  elv_list *timers;
  elv_list *fd_events;
};

struct loop *loop_create(void);

void loop_add_fd(struct loop *loop, int fd, short flags, void *data, void (*callback)(int fd, short flags, void *data));
void loop_add_timer(struct loop *loop, int ms, void *data, void (*callback)(void *data));
void loop_poll(struct loop *loop);

bool loop_remove_fd(struct loop *loop, struct pollfd pfd);
bool loop_remove_timer(struct loop *loop, struct loop_timer *timer);
bool loop_destroy(struct loop *loop);

#endif
