#include "sixpp.h"

#include <errno.h>
#include <fcntl.h>
#include <poll.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/inotify.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

double SPPTreeNode_draw(SPPTreeNode *node, PPProfile *profile, Canvas *canvas, int depth, double x0, double h, double w, int64_t val) {
  Box box = {.height = h, .x0 = x0, .y0 = h*depth};
  box.width = ((double)node->val) / (double)val;
  box.width *= w;
  if (box.width < 0.01)
    return box.width;
  if (!Box_draw(canvas, &box, node->idx)) {
    return box.width;
  }

  for (int i = 0; i < node->sz; ++i)
    x0 += SPPTreeNode_draw(&node->children[i], profile, canvas, depth+1, x0, h, box.width, node->val);
  return box.width;
}

bool SPPTree_draw(SPPTree *tree, PPProfile *profile, Canvas *canvas) {
  SPPTreeNode *node = &tree->root;

  // Sum up the values of the children for the root context
  for (int i = 0; i < node->sz; ++i)
    node->val += node->children[i].val;
  double height = 1.0/((double)tree->depth);

  // Now we're ready to start drawing the nodes
  SPPTreeNode_draw(node, profile, canvas, 0, 0.0, height, 1.0, node->val);
  return true;
}

bool render(char *path, Canvas *canvas, int dfd) {
  PPProfile *profile;
  if (-1 == dfd)
    profile = Profile_frompath(path);
  else
    profile = Profile_frompathat(dfd, path);
  if (!profile) {
    printf("Couldn't open pprof %s\n", path);
    return false;
  }
  SPPTree tree = {0};
  SPPTreeNode_init(&tree.root);
  if (!SPPTree_frompprof(&tree, profile, 0)) {
    printf("Couldn't traverse pprof %s\n", path);
    SPPTreeNode_free(&tree.root);
    return false;
  }

  SPPTree_draw(&tree, profile, canvas);
  Canvas_render(canvas);
  SPPTreeNode_free(&tree.root);
  Profile_free(profile);
  return true;
}

int main(int n, char** v) {
  int width = 2*640, height=2*480;
  SixelContext ctx;
  Canvas canvas;

  // Check input
  if (!n) {
    printf("Must give at least one argument\n");
    return -1;
  }

  // Optional params
  if (n > 2) {
    long buf = atol(v[2]);
    if (buf) {
      width = buf;
      height = buf;
    }
  }
  if (n > 3) {
    long buf = atol(v[3]);
    if (buf)
      width = buf;
  }

  // Initializations
  if (!SixelContext_init(&ctx))
    return -1;
  if (!Canvas_init(&canvas, height, width, &ctx))
    return -1;

  // Check the input.  If it's a file, then go ahead and display it.  If it's a
  // directory, setup an inotify workflow and watch it.
  char *path = v[1];
  struct stat sa = {0};
  if (stat(path, &sa)) {
    if (errno == ENOENT)
      printf("Path %s not found", path);
    else if (errno == EACCES)
      printf("Path %s cannot be accessed", path);
    return -1;
  }

  if (sa.st_mode & S_IFDIR) {
    int ifd = inotify_init();
    int dfd = open(path, O_RDONLY);
    if (-1 == ifd) {
      printf("Could not initialize inotify instance\n");
      return -1;
    }
    int wfd = inotify_add_watch(ifd, path, IN_CLOSE_WRITE);
    if (-1 == wfd) {
      printf("Could not watch the given directory %s\n", path);
      return -1;
    }

    // Sit on the directory and wait
    void *buf = calloc(1, sizeof(struct inotify_event) + 1 + NAME_MAX); // path headroom
    printf("\33[H");
    printf("\033[?1;3;256S" "\033[?8452h" "\033[1;1'z");
    fflush(stdout);
    while(1) {
      struct inotify_event *ie = NULL;
      int n = read(ifd, buf, sizeof(*ie) + 1 + NAME_MAX);
      ie = (struct inotify_event *)buf;
      while (n >= sizeof(*ie) + ie->len) {
        n -= sizeof(*ie) + ie->len;
        char *ext = strrchr(ie->name, '.');
        if (ext && !strcmp(ext, ".pprof")) {
          render(ie->name, &canvas, dfd);
          printf("\33[H");
          fflush(stdout);
        } else {
          printf("Ignoring non-.pprof file %s\n", ie->name);
        }
      }
    }
  } else {
    render(path, &canvas, -1);
  }

//  SPPTree_print(&tree, profile);

  return 0;
}
