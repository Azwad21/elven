#include <poll.h>
#include <stdbool.h>
#include <time.h>

#ifndef _ELVEN_LOOP_H
#define _ELVEN_LOOP_H

#define INT_MAX_TIMER 1000*30

struct loop_timer {
  struct timespec expiry;
  void (*callback)(void *data);
  void *data;
};

struct timers {
  struct loop_timer **items;
  int length;
  int capacity;
};

struct loop {
  struct pollfd *fds;
  struct timers *timers;
};

struct loop *loop_create(void);
void loop_add_timer(struct loop *loop, int ms, void *data, void (*callback)(void *data));

void loop_poll(struct loop *loop);
bool loop_remove_timer(struct loop *loop, struct loop_timer *timer);

#endif
