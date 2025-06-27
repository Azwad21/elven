#include "elven/buffer.h"
#include "elven/client_state.h"
#include "elven/render_context.h"
#include <errno.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>
#include <wayland-client-protocol.h>

static int create_shm_file(void) {
  int retries = 100;
  do {
    --retries;
    char name[64];
    snprintf(name, sizeof(name), "/elven_shm_%ld", time(NULL));

    int fd = shm_open(name, O_RDWR | O_CREAT | O_EXCL, S_IRWXU);
    if (fd >= 0) {
      shm_unlink(name);
      return fd;
    }
  } while (retries > 0 && errno == EEXIST);

  return -1;
}

int shm_get_fd(size_t size) {
  int fd = create_shm_file();
  if (fd < 0) {
    return -1;
  }
  int ret;
  do {
    ret = ftruncate(fd, size);
  } while (ret < 0 && errno == EINTR);
  if (ret < 0) {
    close(fd);
    return -1;
  }
  return fd;
}

static void wl_buffer_release(void *data, struct wl_buffer *wl_buffer) {
  /* Sent by the compositor when it's no longer using this buffer */
  wl_buffer_destroy(wl_buffer);
}

static const struct wl_buffer_listener wl_buffer_listener = {
  .release = wl_buffer_release,
};

void create_buffer(struct Bar *state, struct RenderContext *render_context) {
  render_context->wl_buffer = NULL;

  int stride = state->monitor_width * 4;
  int size = state->monitor_height * stride;

  int fd = shm_get_fd(size);
  if (-1 == fd) {
    render_context->wl_buffer = NULL;
  }
  render_context->data =
    mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
  if (render_context->data == MAP_FAILED) {
    // close(fd);
    render_context->wl_buffer = NULL;
  }

  switch (errno) {
  case EACCES:
    printf("EACCES");
    break;

  case EBADF:
    printf("EBADF");
    break;

  case EINVAL:
    printf("EINVAL");
    break;
  case EMFILE:
    printf("EMFILE");
    break;
  case ENODEV:
    printf("ENODEV");
    break;
  case ENOMEM:
    printf("ENOMEM");
    break;
  case EAGAIN:
    printf("EAGAIN");
    break;
  }

  struct wl_shm_pool *pool = wl_shm_create_pool(state->wl_shm, fd, size);

  render_context->wl_buffer = wl_shm_pool_create_buffer(
    pool, 0, state->monitor_width, state->monitor_height, stride,
    WL_SHM_FORMAT_ARGB8888);
  wl_shm_pool_destroy(pool);
  close(fd);
  wl_buffer_add_listener(render_context->wl_buffer, &wl_buffer_listener, NULL);
}
