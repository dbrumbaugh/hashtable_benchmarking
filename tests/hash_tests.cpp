#include <check.h>
#include <cstdio>
#include <cstdlib>
#include "hashtable.hpp"

uint32_t testhash(uint32_t hval)
{
    hval = ((hval >> 16) ^ hval) * 0x45d9f3b;
    hval = ((hval >> 16) ^ hval) * 0x45d9f3b;
    hval = (hval >> 16) ^ hval;

    return hval;
}

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
    auto *test = new HashTable<uint32_t, int*>(n, &testhash);

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


    fprintf(stderr, "Average chain length is: %lf", test->average_chain());

    delete test;
}
END_TEST


START_TEST(resize)
{
    int n = 20;
    auto *test = new HashTable<int, int>(n);

    for (int i=0; i<15; i++)
        test->insert(i, i);

    ck_assert_int_eq(15, test->length());
    ck_assert_double_eq(15.0 / 20.0, test->load_factor());

    test->insert(15, 15);

    ck_assert_int_eq(16, test->length());
    ck_assert_double_eq(16.0 / 20.0, test->load_factor());

    test->insert(16, 16);

    ck_assert_int_eq(17, test->length());
    ck_assert_double_eq(17.0 / 40.0, test->load_factor());

    for (int i=0; i<17; i++) {
        auto x = test->access(i);
        ck_assert_ptr_nonnull(x);
        ck_assert_int_eq(x->value, i);
    }
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
    tcase_add_test(basic, resize);

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
