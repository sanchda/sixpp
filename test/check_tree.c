#include "sixpp.h"

#include <check.h>
#include <stdlib.h>

START_TEST(tree_init) {
  SPPTreeNode *node = &(SPPTreeNode){0};
  SPPTreeNode *node2;
  ck_assert(SPPTreeNode_init(node));
  ck_assert_ptr_nonnull(node2 = SPPTreeNode_add(node, 0, 10));
  ck_assert_ptr_nonnull(SPPTreeNode_add(node, 0, 10));
  ck_assert_ptr_nonnull(SPPTreeNode_add(node, 2, 10));
  ck_assert_ptr_nonnull(SPPTreeNode_add(node, 2, 10));
  ck_assert_ptr_nonnull(SPPTreeNode_add(node2, 0, 10));
  SPPTreeNode_free(node);
}
END_TEST

Suite * tree_suite() {
  Suite *s = suite_create("Tree");
  TCase *tc_init = tcase_create("Init");

  tcase_add_test(tc_init, tree_init);
  suite_add_tcase(s, tc_init);

  return s;
}

int main() {

int number_failed;
  Suite *s = tree_suite();
  SRunner *sr = srunner_create(s);

  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
