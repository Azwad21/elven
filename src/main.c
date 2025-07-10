#include "elven/bar.h"
#include "elven/registry.h"
#include "elven/wl_output.h"
#include "elven/zwlr_layer_shell.h"
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

  bar.running = true;
  bar_run(&bar);
  bar_destroy(&bar);
  // struct pollfd fds[1];
  // fds[0].fd = wl_display_get_fd(bar.wl_display);
  // fds[0].events = POLLIN;
  //
  // while (1) {
  //   wl_display_flush(bar.wl_display);
  //
  //   int ret = poll(fds, 1, -1);
  //   if (ret > 0) {
  //     if (fds[0].revents & POLLIN) {
  //       wl_display_dispatch(bar.wl_display);
  //     }
  //   }
  // }

  return 0;
}
