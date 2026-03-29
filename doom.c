#define _GNU_SOURCE

#include "display.h"

#include "input.h"

#include <stdio.h>

#include "PureDOOM.h"

static void setup_doom(display_ctx_t *dctx);
void doom_indexed_to_rgb565_fb(const uint8_t *src, uint16_t *dst, int src_w,
                               int src_h, int dst_w, int dst_h,
                               int pitch_bytes);

int main(int argc, char *argv[argc]) {
  puts("Hello, world");

  display_ctx_t dctx;
  display_init("/dev/graphics/fb0", &dctx);

  input_init();

  setup_doom(&dctx);

  int flgs = 0;
  flgs |= DOOM_FLAG_HIDE_MUSIC_OPTIONS;
  flgs |= DOOM_FLAG_HIDE_SOUND_OPTIONS;
  flgs |= DOOM_FLAG_HIDE_MUSIC_OPTIONS;
  flgs |= DOOM_FLAG_MENU_DARKEN_BG;

  doom_init(argc, argv, flgs);

  while (true) {
    input_process();
    doom_update();

    const uint8_t *fb = doom_get_framebuffer(1);
    doom_indexed_to_rgb565_fb(fb, dctx.fb_mem, 320, 200, dctx.width,
                              dctx.height, dctx.pitch);
    display_draw(&dctx);
  }

  display_deinit(&dctx);
  input_deinit();

  return 0;
}

static void setup_doom(display_ctx_t *dctx) {
  printf("width: %d, height: %d\n", dctx->width, dctx->height);
  doom_set_default_int("mouse_move", 0); // Mouse will not move forward
  return;
}

void doom_indexed_to_rgb565_fb(const uint8_t *src, uint16_t *dst, int src_w,
                               int src_h, int dst_w, int dst_h,
                               int pitch_bytes) {
  int dst_stride = pitch_bytes / 2;

  for (int y = 0; y < dst_h; y++) {
    int sy = y * src_h / dst_h;
    uint16_t *d = dst + y * dst_stride;

    for (int x = 0; x < dst_w; x++) {
      int sx = x * src_w / dst_w;
      d[x] = palette565[src[sy * src_w + sx]];
    }
  }
}
