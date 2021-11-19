#include "sixpp.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void sixel_printstatus(SIXELSTATUS status) {
  printf("%s\n%s\n", sixel_helper_format_error(status),
                     sixel_helper_get_additional_message());
}

static int sixel_write(char *buf, int sz, void *arg) {
  return fwrite(buf, 1, sz, (FILE *)arg);
}

bool SixelContext_init(SixelContext *ctx) {
  SIXELSTATUS status;
  status = sixel_output_new(&ctx->output, sixel_write, stdout, NULL);
  if (SIXEL_FAILED(status))
    return false;

  ctx->dither = sixel_dither_get(SIXEL_BUILTIN_XTERM256);
  sixel_dither_set_pixelformat(ctx->dither, SIXEL_PIXELFORMAT_G8);
  return true;
}

bool Canvas_init(Canvas *c, uint64_t h, uint64_t w, SixelContext *ctx) {
  if (!h || !w)
    return false;
  memset(c, 0, sizeof(*c));
  c->data = calloc(1, h*w);
  if (!c->data)
    return false;

  c->height = h;
  c->width = w;
  c->y1 = h;
  c->x1 = w;
  c->ctx = ctx;
  return true;
}
void Canvas_free(Canvas *c) {
  free(c->data);
  memset(c, 0, sizeof(*c));
}
bool Canvas_box_add(Canvas *c, Box *b, char val) {
  // OK, so we decided to have percentile boxes.  That means we need to scale
  // the Box into the Canvas and omit drawing if any of the dimensions are zero.
  uint64_t x0 = 0, y0 = 0, x1 = 0, y1 = 0;
  double w = b->width, h = b->height;

  // Scaling
  if (w < 0.0 || h < 0.0)
    return false;
  w = w > 1.0 ? 1.0 : w;
  h = h > 1.0 ? 1.0 : h;

  // Compact dimensions?
  if (w * c->width < 1.0 || h * c->height < 1.0)
    return false;

  // Get new dimensions
  x0 = b->x0 * c->width;
  y0 = b->y0 * c->height;
  x1 = x0 + w * c->width;
  y1 = x1 + h * c->height;

  if (x1 > c->x1)
    x1 = c->x1;
  if (y1 > c->y1)
    y1 = c->y1;

  // Draw interior of box
  for (int i = x0; i < x1; ++i)
    for (int j = y0; j < y1; ++j)
      c->data[c->width * j + i] = val;

  // Draw border
  for (int i = x0; i < x1; ++i) {
    c->data[c->width * y0 + i] = 0xff;
    c->data[c->width * y1 + i] = 0xff;
  }
  for (int j = y0; j < y1; ++j) {
    c->data[c->width * j + x0] = 0xff;
    c->data[c->width * j + x1] = 0xff;
  }

  return true;
}

bool Canvas_draw(Canvas *c) {
  SIXELSTATUS status;
  sixel_dither_t *dither = c->ctx->dither;
  sixel_output_t *output = c->ctx->output;
  status = sixel_encode(c->data, c->width, c->height, 0xF0, dither, output);
  if (SIXEL_FAILED(status)) {
    sixel_printstatus(status);
    return false;
  }
  return true;
}
