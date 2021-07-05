#include <check.h>
#include <stdio.h>
#include "hashtable.hpp"

START_TEST(construct_hashtable)
{
    auto test = new HashTable<int, void*>();

    ck_assert_ptr_ne(test, NULL);
    ck_assert_int_eq(test->length(), 0);
    delete test;
}
END_TEST


/*
 * I'm really not sure how to test this to verify that the
 * destructor is working properly.
 *
 */
START_TEST(destroy_hashtable)
{
    auto *test = new HashTable<int, void*>();

    delete test;
    // tests go here. Once I come up with some...
}
END_TEST


START_TEST(simple_insert)
{
    auto *test = new HashTable<int, void*>();
    int *test_val = (int *) calloc(1, sizeof(int));
    *test_val = 100;

    
    // Insert returns pointer to inserted value
    auto res = test->insert(5, test_val);
    ck_assert_ptr_eq(res->value, test_val);

    // size of table is now 1
    ck_assert_int_eq(test->length(), 1);

    // Duplicate insert returns pointer to existing value
    auto res2 = test->insert(5, test_val);
    ck_assert_ptr_eq(res2->value, test_val);

    // size of table is still 1
    ck_assert_int_eq(test->length(), 1);

    // Accessing inserted key results in correct value
    auto res3 = test->access(5); 
    ck_assert_ptr_eq(res3->value, test_val);


    // Accessing invalid key results in NULL
    void *res4 = test->access(9);
    ck_assert_ptr_eq(res4, nullptr);

    delete test;
}
END_TEST


START_TEST(simple_delete)
{
    auto *test = new HashTable<int, double>();
    double val = 1.4;

    test->insert(5, val);
    auto temp = test->access(5);
    ck_assert_double_eq(val, temp->value);

    int pre_size = test->length();

    test->remove(5);
    auto temp2 = test->access(5);

    ck_assert_ptr_eq(temp2, nullptr);

    int post_size = test->length();

    ck_assert_int_eq(pre_size - 1, post_size);

}
END_TEST


START_TEST(bulk_insert)
{
    int n = 2048;
    auto *test = new HashTable<int, int*>(n);

    int *test_data = (int *) calloc(2*n, sizeof(int));
    for (int i=0; i<2*n; i++) {
        test_data[i] = rand();
        test->insert(i, &(test_data[i]));
        ck_assert_int_eq(test->length(), i+1);
    }

    ck_assert_int_eq(test->length(), 2*n);



    for (int i=0; i<2*n; i++) {
        auto res = test->access(i);
        ck_assert_ptr_ne(res, nullptr);
        ck_assert_int_eq(*(res->value), test_data[i]);
    }

    delete test;
}
END_TEST


START_TEST(hash_collision)
{
    int n = 2048;


    int val1=100;
    int val2=105;

    auto *test = new HashTable<int, int>(n);

    test->insert(0, val1);
    test->insert(n, val2);

    ck_assert_int_eq(test->length(), 2);

    auto res1 = test->access(0);
    auto res2 = test->access(n);

    ck_assert_int_eq(res1->value, val1);
    ck_assert_int_eq(res2->value, val2);
    

    delete test;
}
END_TEST
    


Suite *test_suite()
{
    Suite *suite = suite_create("IV_FixedType Hashtable");

    // Test the basic functionality
    TCase *basic = tcase_create("basic");
    tcase_add_test(basic, construct_hashtable);
    tcase_add_test(basic, destroy_hashtable);
    tcase_add_test(basic, simple_insert);
    tcase_add_test(basic, simple_delete);

    tcase_add_test(basic, hash_collision);

    tcase_add_test(basic, bulk_insert);

    // TODO: Add stress testing
    TCase *stress = tcase_create("stress");

    suite_add_tcase(suite, basic);
    suite_add_tcase(suite, stress);

    return suite;
}

int run_test_suite()
{
    int failed = 0;
    Suite *suite = test_suite();
    SRunner *runner = srunner_create(suite);

    srunner_run_all(runner, CK_VERBOSE);
    failed = srunner_ntests_failed(runner);
    srunner_free(runner);

    return failed;
}


int main() 
{
    int failed = run_test_suite();

    return (failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
