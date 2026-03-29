#include "display.h"

#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <unistd.h>

#ifndef PAGE_SIZE
#define PAGE_SIZE 65536
#endif

static inline uint64_t roundUpToPageSize(uint64_t x) {
  return (x + (PAGE_SIZE - 1)) & ~(PAGE_SIZE - 1);
}

/*
 * @path = usually /dev/graphics/fb0
 * @out = you have to allocate
 */
int display_init(const char *path, display_ctx_t *out) {
  if (!out || !path)
    return -1;
  memset(out, 0, sizeof(*out));

  int fd = open(path, O_RDWR, 0);
  if (fd < 0) {
    perror("open fb");
    return -1;
  }
  out->fd = fd;

  if (ioctl(fd, FBIOGET_FSCREENINFO, &out->finfo) == -1) {
    perror("FBIOGET_FSCREENINFO");
    goto fail;
  }

  if (ioctl(fd, FBIOGET_VSCREENINFO, &out->vinfo) == -1) {
    perror("FBIOGET_VSCREENINFO");
    goto fail;
  }

  struct fb_var_screeninfo v = out->vinfo;

  v.reserved[0] = 0;
  v.reserved[1] = 0;
  v.reserved[2] = 0;
  v.xoffset = 0;
  v.yoffset = 0;
  v.activate = FB_ACTIVATE_NOW;

  /* force RGB565 */
  v.bits_per_pixel = 16;
  v.red.offset = 11;
  v.red.length = 5;
  v.green.offset = 5;
  v.green.length = 6;
  v.blue.offset = 0;
  v.blue.length = 5;
  v.transp.offset = 0;
  v.transp.length = 0;

  if (ioctl(fd, FBIOPUT_VSCREENINFO, &v) == -1) {
    perror("FBIOPUT_VSCREENINFO");
    goto fail;
  }

  out->vinfo = v;

  out->width = v.xres;
  out->height = v.yres;
  out->pitch = out->finfo.line_length;

  /* fallback DPI if driver doesn't provide */
  if (v.width <= 0 || v.height <= 0) {
    v.width = (v.xres * 25.4f) / 160.0f + 0.5f;
    v.height = (v.yres * 25.4f) / 160.0f + 0.5f;
  }

  out->xdpi = (v.xres * 25.4f) / v.width;
  out->ydpi = (v.yres * 25.4f) / v.height;

  out->fb_size = roundUpToPageSize(out->finfo.line_length * v.yres_virtual);

  out->fb_mem =
      mmap(NULL, out->fb_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

  if (out->fb_mem == MAP_FAILED) {
    perror("mmap fb");
    goto fail;
  }

  memset(out->fb_mem, 0, out->fb_size);

  return 0;

fail:
  if (fd >= 0)
    close(fd);
  return -1;
}

void display_deinit(display_ctx_t *ctx) {
  if (!ctx)
    return;

  if (ctx->fb_mem && ctx->fb_mem != MAP_FAILED) {
    munmap(ctx->fb_mem, ctx->fb_size);
  }

  if (ctx->fd >= 0) {
    close(ctx->fd);
  }

  memset(ctx, 0, sizeof(*ctx));
}

int display_draw(display_ctx_t *ctx) {
  ctx->vinfo.activate = FB_ACTIVATE_FORCE;
  if (0 > ioctl(ctx->fd, FBIOPUT_VSCREENINFO, &ctx->vinfo)) {
    perror("FBIOPUT_VSCREENINFO failed : error on refresh");
    return -1;
  }

  return 0;
}
