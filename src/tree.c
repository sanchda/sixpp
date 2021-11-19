#include "sixpp.h"

#include <stdio.h>
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
  memcpy(new_children, node->children, sizeof(*node)*node->sz);
  free(node->children);
  node->children = new_children;
  node->capacity += COHORT_SIZE;
  return true;
}

SPPTreeNode *SPPTreeNode_add(SPPTreeNode *node, int64_t idx, uint64_t val) {
  for (int i = 0; i < node->sz; i++) {
    if (node->children[i].idx == idx) {
      node->children[i].val += val;
      return &node->children[i];
    }
  }

  // If we're here, then we didn't find an existing child
  if (node->sz == node->capacity) {
    if (!SPPTreeNode_grow(node)) {
      printf("Failure to grow node\n");
      return NULL;
    }
  }
  if (!SPPTreeNode_init(&node->children[node->sz])) {
    printf("Failure to init node\n");
    return NULL;
  }
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

bool SPPTree_fromsample(SPPTree *tree, PPSample *sample, int n) {
  SPPTreeNode *node = &tree->root;
  SPPTreeNode *next = NULL;

  if (n < 0 || n >= sample->n_value)
    return false;
  int64_t val = sample->value[n];
  for (int i = 0; i < sample->n_location_id; ++i) {
    next = SPPTreeNode_add(node, sample->location_id[i], val);
    if (!next) {
      // This is bad.  We've populated the partial tree, but don't have a valid
      // successor.  This is probably because we can't allocate space.  Fail.
      return false;
    }
    node = next;
  }

  return true;
}

bool SPPTree_frompprof(SPPTree *tree, PPProfile *pprof, int n) {
  // n indexes into values
  for (int i = 0; i < pprof->n_sample; ++i) {
    if (!SPPTree_fromsample(tree, pprof->sample[i], n)) {
      printf("Failed to process sample %d\n", i);
      return false;
    }
  }
  return true;
}

void SPPTreeNode_print(SPPTreeNode *node, PPProfile *profile, int depth) {
  // Print this node
  if (!node->idx) {
    // Technically bad if depth is nonzero -- ignore for now
    printf("%*s<%s>[%d]: %lu\n", 0, "", "[ROOT]", 0, 0L);
  } else {
    PPLocation *loc = profile->location[node->idx - 1];
    for (int i = 0; i < loc->n_line; i++) {
      int64_t fun_id = loc->line[i]->function_id;
      int64_t str_id = profile->function[fun_id-1]->name;
      int64_t sys_id = profile->function[fun_id-1]->system_name;
      int64_t id = str_id ? str_id : sys_id;
      char *name;
      if (!id)
        name = "UND";
      else
        name = profile->string_table[id];
      printf("%*s<%s>[%d]: %lu\n", depth, "", name, i, node->idx);
    }
  }

  // Print the children
  for (int i = 0; i < node->sz; i++)
    SPPTreeNode_print(&node->children[i], profile, depth + 1);
}

void SPPTree_print(SPPTree *tree, PPProfile *profile) {
  SPPTreeNode *node = &tree->root;
  SPPTreeNode_print(node, profile, 0);
}
