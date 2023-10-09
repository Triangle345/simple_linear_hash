#include "slh.h"
#include "acutest/include/acutest.h"



void stress_test_hash(void)
{
    slh_ctx ht = slh_new();
    int i1 = 5;
    slh_insert(ht, djb2("testing"), &i1);
    TEST_CHECK(slh_len(ht) == 1);
    
    for (int i = 0; i< 1000000;i++) {
        int* tmp = (int*)calloc(1, sizeof(int));
        *tmp = i;
        char buf[20] = {0};
        sprintf(buf, "testing%d", i);
        slh_insert(ht, djb2(buf), tmp);
    }

    // TEST_CHECK_(_ht->size == 2097152,"size=%ld\n", _ht->size);
    TEST_CHECK_(slh_len(ht) == 1000001, "items=%ld\n", slh_len(ht));  
    slh_free(ht);  
}
void test_collision (void) {
    //'testing6', 'testing150', 'testing231'
    slh_ctx ht = slh_new();
    int i1 = 6;
    int i2 = 150;
    int i3 = 231;
    slh_insert(ht, djb2("testing6"), &i1);
    slh_insert(ht, djb2("testing150"), &i2);
    slh_insert(ht, djb2("testing231"), &i3);

    TEST_CHECK(slh_len(ht) == 3);

    int* ret = slh_get(ht, djb2("testing150"));
    TEST_CHECK(*ret == 150);
    ret = slh_get(ht, djb2("testing150"));
    TEST_CHECK(*ret == 150);
    ret = slh_remove(ht, djb2("testing150"));
    TEST_CHECK(slh_len(ht) == 2);
    TEST_CHECK(*ret == 150);

    ret = slh_get(ht, djb2("testing150"));
    TEST_CHECK(ret == NULL);

    ret = slh_get(ht, djb2("testing231"));
    TEST_CHECK(*ret == 231);
    ret = slh_get(ht, djb2("testing6"));
    TEST_CHECK(*ret == 6);
    slh_free(ht);
}

void test_collision_edge (void) {
    //Collision: ['testing147', 'testing228', 'testing309']:idx 511
    slh_ctx ht = slh_new();
    int i1 = 147;
    int i2 = 228;
    int i3 = 309;
    slh_insert(ht, djb2("testing147"), &i1);
    slh_insert(ht, djb2("testing228"), &i2);
    slh_insert(ht, djb2("testing309"), &i3);

    TEST_CHECK(slh_len(ht) == 3);

    int* ret = slh_get(ht, djb2("testing228"));
    TEST_CHECK(*ret == 228);
    ret = slh_get(ht, djb2("testing228"));
    TEST_CHECK(*ret == 228);
    ret = slh_remove(ht, djb2("testing228"));
    TEST_CHECK(slh_len(ht) == 2);
    TEST_CHECK(*ret == 228);

    ret = slh_get(ht, djb2("testing228"));
    TEST_CHECK(ret == NULL);

    ret = slh_get(ht, djb2("testing309"));
    TEST_CHECK(*ret == 309);
    ret = slh_get(ht, djb2("testing147"));
    TEST_CHECK(*ret == 147);
    slh_free(ht);
}

void test_remove(void) {
    slh_ctx ht = slh_new();
    // Ctx* _ht = ht;
    int i1 = 5;
    slh_insert(ht, djb2("testing"), &i1);
    TEST_CHECK(slh_len(ht) == 1);

    int * ret = slh_remove(ht, djb2("testing"));
    TEST_CHECK_(*ret == 5, "ret==%ld\n",*ret);
    TEST_CHECK(slh_len(ht) == 0);
    slh_free(ht);

}

void test_overwrite(void)
{
    slh_ctx ht = slh_new();
    // Ctx* _ht = ht;
    int i1 = 5;
    int i2 = 7;
    int* ret = slh_insert(ht, djb2("testing"), &i1);
    TEST_CHECK(slh_len(ht) == 1);

    // size_t idx = calc_idx(_ht,djb2("testing"));

     int* dat = slh_get(ht, djb2("testing"));
    TEST_CHECK(*dat == 5);
    TEST_CHECK(ret == NULL);

    ret = slh_insert(ht, djb2("testing"), &i2);
    // dat = _ht->ht[idx].data;
    TEST_CHECK(*ret == 5);

    free(ht);

}

TEST_LIST = {
{ "stress test", stress_test_hash },
{"overwrite", test_overwrite},
{"remove test", test_remove},
{"test collision mid", test_collision},
{"test collision edge", test_collision_edge},
{ NULL, NULL }
};