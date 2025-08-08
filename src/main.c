#include "bar.h"
#include <errno.h>
#include <poll.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <wayland-client.h>
#include <wayland-util.h>

static struct elv_bar bar;

void sig_handle_int(int signum) {
  bar.running = false;
}

int main(void) {

  if (signal(SIGINT, sig_handle_int) == SIG_ERR) {
    perror("Failed to attach sig kill handler: ");
  }

  if (!bar_setup(&bar)) {
    perror("Failed to setup bar\n");
    exit(EXIT_FAILURE);
  }

  // printf("Printing fd capacity: %d\n", bar.eventloop->fd_capacity);

  bar.running = true;
  bar_run(&bar);
  // bar_destroy(&bar);

  return 0;
}
