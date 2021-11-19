#include "sixpp.h"

#include <stdlib.h>
#include <string.h>

#define COHORT_SIZE 32

bool SPPTreeNode_init(SPPTreeNode *node) {
  memset(node, 0, sizeof(*node));
  SPPTreeNode *children = calloc(COHORT_SIZE, sizeof(*children));
  if (!children) {
    free(node);
    free(children);
    return false;
  }
  node->children = children;
  node->sz = 0;
  node->capacity = COHORT_SIZE;
  return true;
}

bool SPPTreeNode_grow(SPPTreeNode *node) {
  SPPTreeNode *new_children;
  new_children = calloc(COHORT_SIZE + node->capacity, sizeof(*node->children));
  if (!new_children)
    return false;

  // Copy and replace
  memcpy(new_children, node->children, node->sz);
  free(node->children);
  node->children = new_children;
  return true;
}

SPPTreeNode *SPPTreeNode_add(SPPTreeNode *node, uint64_t idx, uint64_t val) {
  for (int i = 0; i < node->sz; i++) {
    if (node->children[i].idx == idx) {
      node->children[i].val += val;
      return &node->children[i];
    }
  }

  // If we're here, then we didn't find an existing child
  if (node->sz == node->capacity) {
    if (!SPPTreeNode_grow(node))
      return NULL;
  }
  if (!SPPTreeNode_init(&node->children[node->sz]))
    return NULL;
  SPPTreeNode *ret = &node->children[node->sz];
  node->children[node->sz].val = val;
  node->children[node->sz].idx = idx;
  ++node->sz;
  return ret;
}

void SPPTreeNode_free(SPPTreeNode *node) {
  for (int i = 0; i < node->sz; i++)
    SPPTreeNode_free(&node->children[i]);
  free(node->children);
}

bool SPPTree_fromsample(PPSample *sample, SPPTree *tree, int n) {
  SPPTreeNode *node = &tree->root;
  SPPTreeNode *next = NULL;

  if (n < 0 || n >= sample->n_value)
    return false;
  int64_t val = sample->value[n];
  for (int i = 0; i < sample->n_location_id; ++i) {
    next = SPPTreeNode_add(node, sample->location_id, val);
    if (!next) {
      // This is bad.  We've populated the partial tree, but don't have a valid
      // successor.  This is probably because we can't allocate space.  Fail.
      return false;
    }
    node = next;
  }

  return true;
}

bool SPPTree_frompprof(PPProfile *pprof, SPPTree *tree, int n) {
  // n indexes into values
  for (int i = 0; i < pprof->n_sample; ++i)
    SPPTree_fromsample(pprof->sample[i], tree, n);
}
