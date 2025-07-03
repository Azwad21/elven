#include "elven/pool-buffer.h"
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>
#include <wayland-client-core.h>
#include <wayland-client-protocol.h>

static int shm_get_fd(void) {
  int retries = 100;
  do {
    --retries;
    char name[64];
    snprintf(name, sizeof(name), "/elven-shm-%ld", time(NULL));

    int fd = shm_open(name, O_RDWR | O_CREAT | O_EXCL, S_IRWXU);
    if (fd >= 0) {
      shm_unlink(name);
      return fd;
    }
  } while (retries > 0 && errno == EEXIST);

  return -1;
}

int shm_get_fd_a(size_t size) {
  int fd = shm_get_fd();
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

static void handle_buffer_release(void *data, struct wl_buffer *buffer) {
}

static struct wl_buffer_listener buffer_listener = {
  .release = handle_buffer_release,
};

static int create_buffer(struct render_context *render_ctx, struct wl_shm *shm,
  uint32_t width, uint32_t height) {
  int stride = width * 4;
  int size = stride * height;

  int fd = shm_get_fd();
  if (fd < 0) {
    return 0;
  }

  if (!ftruncate(fd, size)) {
    perror("On create_buffer: ftruncate");
    return 0;
  }

  void *data = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

  struct wl_shm_pool *pool = wl_shm_create_pool(shm, fd, size);

  render_ctx->buffer = wl_shm_pool_create_buffer(
    pool, 0, width, height, stride, WL_SHM_FORMAT_ARGB8888);
  wl_shm_pool_destroy(pool);
  close(fd);
  wl_buffer_add_listener(
    render_ctx->buffer, &buffer_listener, render_ctx->buffer);
  return 1;
}

struct render_context *get_buffer(
  struct wl_shm *shm, uint32_t width, uint32_t height) {
  struct render_context *render_ctx = NULL;
  if (!create_buffer(render_ctx, shm, width, height)) {
    printf("Error: get_buffer");
    exit(EXIT_FAILURE);
  }
  return render_ctx;
}
