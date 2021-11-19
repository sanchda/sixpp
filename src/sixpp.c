#include "sixpp.h"

#include <stdio.h>

static int sixel_write(char *data, int size, void *priv) {
  return fwrite(data, 1, size, (FILE *)priv);
}

int main(int n, char** v) {
  SixelContext ctx;
  Canvas canvas;
  if (!SixelContext_init(&ctx))
    return -1;
  if (!Canvas_init(&canvas, 480, 640, &ctx))
    return -1;

  Box box = {0};
  box.width = .25;
  box.height = .25;
  box.x0 = 0.2;
  box.y0 = 0.6;
  if (!Canvas_box_add(&canvas, &box, 0x11))
    return -1;

  if (!Canvas_draw(&canvas))
    return -1;

  printf("Good!\n");
  return 0;
  PPProfile *profile = Profile_frompath(v[1]);
  if (!profile) {
    printf("Couldn't open pprof %s\n", v[1]);
    return -1;
  }
  SPPTree tree = {0};
  SPPTreeNode_init(&tree.root);
  if (!SPPTree_frompprof(&tree, profile, 0)) {
    printf("Couldn't traverse pprof %s\n", v[1]);
//    return -1;
  }
  SPPTree_print(&tree, profile);
  return 0;
}
