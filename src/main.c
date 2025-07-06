#include "elven/bar.h"
#include "elven/registry.h"
#include "elven/wl_output.h"
#include "elven/zwlr_layer_shell.h"
#include <errno.h>
#include <poll.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <wayland-client.h>
#include <wayland-util.h>

int main(void) {
  struct elv_bar bar;

  if (!bar_setup(&bar)) {
    perror("Failed to setup bar\n");
    exit(EXIT_FAILURE);
  }

  bar_run(&bar);
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
