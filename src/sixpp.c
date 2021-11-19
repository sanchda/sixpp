#include "sixpp.h"

#include <stdio.h>

double SPPTreeNode_draw(SPPTreeNode *node, PPProfile *profile, Canvas *canvas, int depth, double x0, double h, int64_t val) {
  Box box = {.height = h, .x0 = x0, .y0 = h*depth, .height = h};
  box.width = ((double)node->val) / (double)val;
  Box_draw(canvas, &box, node->idx & 255);

  for (int i = 0; i < node->sz; ++i)
    x0 += SPPTreeNode_draw(&node->children[i], profile, canvas, depth+1, x0, h, node->val);
  printf(".");
}

bool SPPTree_draw(SPPTree *tree, PPProfile *profile, Canvas *canvas) {
  SPPTreeNode *node = &tree->root;

  // Sum up the values of the children for the root context
  for (int i = 0; i < node->sz; ++i)
    node->val += node->children[i].val;
  double height = 1.0/((double)tree->depth);

  // Now we're ready to start drawing the nodes
  SPPTreeNode_draw(node, profile, canvas, 0, 0.01, height, node->val);
  return true;
}

int main(int n, char** v) {
  SixelContext ctx;
  Canvas canvas;
  if (!SixelContext_init(&ctx))
    return -1;
  if (!Canvas_init(&canvas, 2*480, 2*640, &ctx))
    return -1;

  PPProfile *profile = Profile_frompath(v[1]);
  if (!profile) {
    printf("Couldn't open pprof %s\n", v[1]);
    return -1;
  }
  SPPTree tree = {0};
  SPPTreeNode_init(&tree.root);
  if (!SPPTree_frompprof(&tree, profile, 0)) {
    printf("Couldn't traverse pprof %s\n", v[1]);
    return -1;
  }

  SPPTree_draw(&tree, profile, &canvas);
  Canvas_render(&canvas);
  return 0;
}
