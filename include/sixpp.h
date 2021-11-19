#include <stdbool.h>

#include <sixel.h>
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
  struct SPPTreeNode *children; // An array of nodes
  uint64_t capacity; // Capacity to hold children
  uint64_t sz;  // number of existing children
  int64_t idx; // the location_id
  uint64_t val; // Aggregated value
} SPPTreeNode;

typedef struct SPPTree {
  SPPTreeNode root;
  uint64_t depth;
} SPPTree;

bool SPPTreeNode_init(SPPTreeNode *node);
bool SPPTreeNode_grow(SPPTreeNode *node);
SPPTreeNode *SPPTreeNode_add(SPPTreeNode *node, int64_t idx, uint64_t val);
void SPPTreeNode_free(SPPTreeNode *node);
void SPPTree_print(SPPTree *node, PPProfile *profile);
void SPPTreeNode_print(SPPTreeNode *tree, PPProfile *profile, int depth);

bool SPPTree_fromsample(SPPTree *tree, PPSample *sample, int n);
bool SPPTree_frompprof(SPPTree *tree, PPProfile *pprof, int n);

PPProfile *Profile_fromfd(int fd);
PPProfile *Profile_frompath(char *path);
PPProfile *Profile_frompathat(int dfd, char *path);
void Profile_free(PPProfile *profile);

// Data structures for Sixel stuff

typedef struct SixelContext {
  sixel_encoder_t *enc;
  sixel_dither_t *dither;
  sixel_output_t *output;
} SixelContext;
// Canvas has slightly duplicatory fields, but whatever
typedef struct Canvas {
  SixelContext *ctx;
  uint64_t width;
  uint64_t height;
  int x0, y0; // upper-left corner of convas
  int x1, y1; // lower-right corner of canvas
  char *data;
} Canvas;

typedef struct Box {
  double width;
  double height;
  double x0, y0; // corner as a fraction of the convas
} Box;

void sixel_printstatus(SIXELSTATUS status);

bool SixelContext_init(SixelContext *ctx);

bool Box_draw(Canvas *c, Box *b, char val);
bool Canvas_init(Canvas *c, uint64_t h, uint64_t w, SixelContext *ctx);
void Canvas_clear(Canvas *c);
void Canvas_free(Canvas *c);
bool Canvas_render(Canvas *c);
