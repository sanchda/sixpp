#include <stdbool.h>

#include "sixel.h"
#include "proto/pprof.pb-c.h"

// Make the pprof type names shorter
#define MINPP(x) typedef Perftools__Profiles__##x PP##x
MINPP(Profile);
MINPP(ValueType);
MINPP(Sample);
MINPP(Label);
MINPP(Mapping);
MINPP(Location);
MINPP(Line);
MINPP(Function);

/*
 * The top-level Profile object of a pprof represents a list of stack traces
 * and their corresponding weights.  Formally speaking, a pprof represents a
 * forest rather than an individual tree, since there's no data-level
 * requirement for all call chains to terminate at the same root.
 *
 * Moreover, there's no requirement that a given stack trace be unique.  It is
 * conventional in some ecosystems to aggregate values, but the plethora of
 * trace-level tags means there's a lot of duplication.  Accordingly, I think we
 * should assume that the burden is on this library to perform aggregation.
 *
 * Consider the following analysis
 * Flamegraph generation
 * Drawing starts from the implicit root (collapse the forest to a common
 * ancestor node), at each level drawing a box at a given offset (however we
 * compute that!) with a width representing the sum of all children.
 *
 * OK, so let's do this.  We'll represent the stack traces as a weighted tree
 * which just loosely indexes into the underlying pprof.  At every step, we'll
 * increment the value of the node by the value of the sample.
 *
 */

// TODO would it be better to put the children in common storage in an LSMT?
typedef struct SPPTreeNode {
  struct SPPTreeNode **children;
  uint64_t capacity; // Capacity to hold children
  uint64_t sz;  // number of existing children
  uint64_t idx; // the location_id
  uint64_t val; // Aggregated value
} SPPTreeNode;

bool SPPTreeNode_init(SPPTreeNode *node);
bool SPPTreeNode_add(SPPTreeNode *node, uint64_t idx, uint64_t val);
