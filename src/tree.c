#include "sixpp.h"

#define COHORT_SIZE 32

bool SPPTreeNode_init(SPPTreeNode *node) {
  memset(node, 0, sizeof(*node));
  node = calloc(1, sizeof(SPPTreeNode));
  SPPTreeNode **children = calloc(COHORT_SIZE, sizeof(*children));
  if (!node || !children) {
    free(node);
    free(children);
    return false;
  }
  node->children = children;
  node->sz = 0;
  node->capacity = COHORT_SIZE;
  return true;
}

bool SPPTreeNode_add(SPPTreeNode *node, uint64_t idx, uint64_t val) {
  for (int i = 0; i < node->sz; i++) {
    if (node->children[i].idx == idx) {
      node->children[i].val += val;
      return true;
    }
  }

  // If we're here, then we didn't find an existing child
  if (node->sz == node->capacity)
    SPPTreeNode_grow(node);
  if (!SPPTreeNode_init(node->children[node->sz]))
    return false;
  children[node->sz].val = val;
  children[node->sz].idx = idx;
  ++node->sz;
  return true;
}
