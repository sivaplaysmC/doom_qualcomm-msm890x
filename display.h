#pragma once

#include <linux/fb.h>
#include <stdint.h>

typedef struct display_ctx_t {
  int fd;

  struct fb_fix_screeninfo finfo;
  struct fb_var_screeninfo vinfo;

  uint16_t *fb_mem;
  uint64_t fb_size;

  uint32_t width;
  uint32_t height;
  uint32_t pitch; // line_length in bytes

  float xdpi;
  float ydpi;
} display_ctx_t;

/* init framebuffer display */
int display_init(const char *path, display_ctx_t *out);

/* cleanup framebuffer display */
void display_deinit(display_ctx_t *ctx);

int display_draw(display_ctx_t *ctx);
