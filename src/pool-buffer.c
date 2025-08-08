#include "pool-buffer.h"
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

    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    char name[64];
    pid_t pid = getpid();
    snprintf(name, sizeof(name), "/elven-shm-%ld-%d", ts.tv_nsec, pid);

    int fd = shm_open(name, O_RDWR | O_CREAT | O_EXCL, S_IRWXU);

    if (fd >= 0) {
      shm_unlink(name);
      return fd;
    }
  } while (retries > 0 && errno == EEXIST);

  return -1;
}

static void handle_buffer_release(void *data, struct wl_buffer *buffer) {
  // munmap the pool data
  struct pool_buffer *pool_buffer = data;
  pool_buffer->busy = false;

  printf("Buffer release called\n");
}

static struct wl_buffer_listener buffer_listener = {
  .release = handle_buffer_release,
};

static int create_buffer(struct pool_buffer *pool_buffer, struct wl_shm *shm,
  uint32_t width, uint32_t height) {
  pool_buffer->width = width;
  pool_buffer->height = height;
  pool_buffer->stride = width * 4;
  pool_buffer->busy = false;
  int size = pool_buffer->stride * height;

  int fd = shm_get_fd();
  if (fd < 0) {
    return 0;
  }

  int ret;
  do {
    ret = ftruncate(fd, size);
  } while (ret < 0 && errno == EAGAIN);

  if (ret < 0) {
    perror("On creatt_buffer: ftruncate");
    return 0;
  }

  pool_buffer->pool_data =
    mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

  struct wl_shm_pool *pool = wl_shm_create_pool(shm, fd, size);

  pool_buffer->buffer = wl_shm_pool_create_buffer(
    pool, 0, width, height, pool_buffer->stride, WL_SHM_FORMAT_ARGB8888);
  wl_shm_pool_destroy(pool);
  close(fd);
  wl_buffer_add_listener(pool_buffer->buffer, &buffer_listener, pool_buffer);
  return 1;
}

struct pool_buffer *get_buffer(
  struct wl_shm *shm, uint32_t width, uint32_t height) {
  struct pool_buffer *pool_buffer = malloc(sizeof *pool_buffer);
  if (!create_buffer(pool_buffer, shm, width, height)) {
    printf("Error: get_buffer");
    exit(EXIT_FAILURE);
  }
  return pool_buffer;
}

void destroy_buffer(struct pool_buffer *pool_buffer) {

  if (pool_buffer->buffer) {
    wl_buffer_destroy(pool_buffer->buffer);
  }
  munmap(pool_buffer->pool_data, pool_buffer->stride * pool_buffer->height);
  free(pool_buffer);
  pool_buffer = NULL;
}
