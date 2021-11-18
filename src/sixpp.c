#include "sixel.h"

#include <stdio.h>

static int sixel_write(char *data, int size, void *priv) {
  return fwrite(data, 1, size, (FILE *)priv);
}

int main() {
  sixel_output_t *output = NULL;
  sixel_dither_t *dither = NULL;
  SIXELSTATUS status = sixel_output_new(&output, sixel_write, stdout, NULL);
}
