#include "sixpp.h"

#include <stdio.h>

static int sixel_write(char *data, int size, void *priv) {
  return fwrite(data, 1, size, (FILE *)priv);
}

int main(int n, char** v) {
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
