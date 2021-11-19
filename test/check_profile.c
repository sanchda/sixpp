#include "sixpp.h"

#include <check.h>
#include <stdlib.h>

START_TEST(profile_init) {
  ck_assert_ptr_nonnull(Profile_frompath("test.pprof"));
}
END_TEST

Suite * profile_suite() {
  Suite *s = suite_create("profile");
  TCase *tc_init = tcase_create("Init");

  tcase_add_test(tc_init, profile_init);
  suite_add_tcase(s, tc_init);

  return s;
}

int main() {

int number_failed;
  Suite *s = profile_suite();
  SRunner *sr = srunner_create(s);

  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
