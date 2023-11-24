#include "slh.h"
#include "acutest/include/acutest.h"
#include <stddef.h>
#include <string.h>



void stress_test_hash(void)
{
    slh_ctx ht = slh_new(sizeof(int));
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
    slh_ctx ht = slh_new(sizeof(int));
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
    bool ret_bool = slh_remove(ht, djb2("testing150"));
    TEST_CHECK(slh_len(ht) == 2);
    TEST_CHECK(ret_bool == true);

    ret_bool = slh_remove(ht, djb2("lsjf"));
    TEST_CHECK(slh_len(ht) == 2);
    TEST_CHECK(ret_bool == false);

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
    slh_ctx ht = slh_new(sizeof(int));
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
    bool ret_bool = slh_remove(ht, djb2("testing228"));
    TEST_CHECK(slh_len(ht) == 2);
    TEST_CHECK(ret_bool == true);

    ret = slh_get(ht, djb2("testing228"));
    TEST_CHECK(ret == NULL);

    ret = slh_get(ht, djb2("testing309"));
    TEST_CHECK(*ret == 309);
    ret = slh_get(ht, djb2("testing147"));
    TEST_CHECK(*ret == 147);
    slh_free(ht);
}

void test_remove(void) {
    slh_ctx ht = slh_new(sizeof(int));
    // Ctx* _ht = ht;
    int i1 = 5;
    slh_insert(ht, djb2("testing"), &i1);
    TEST_CHECK(slh_len(ht) == 1);

    bool ret_bool = slh_remove(ht, djb2("testing"));
    TEST_CHECK_(ret_bool == true, "ret==%d\n",ret_bool);
    TEST_CHECK(slh_len(ht) == 0);
    slh_free(ht);

}

typedef struct _teststruct
{
    int a;
    size_t b;
    char c[12];
} teststruct;

void test_custom_type(void) {
    slh_ctx ht = slh_new(sizeof(teststruct));
    // Ctx* _ht = ht;
    teststruct testitem = {10,20,0};
    strcat_s(testitem.c, 12, "testing");
    slh_insert(ht, djb2("testing"), &testitem);
    TEST_CHECK(slh_len(ht) == 1);

    teststruct *item = (teststruct*)slh_get(ht, djb2("testing"));
    TEST_CHECK_(item->a == 10, "ret==%d\n",item->a);
    TEST_CHECK_(item->b == 20, "ret==%ld\n",item->b);
    TEST_CHECK_(strcmp(item->c, "testing") == 0, "ret==%s\n",item->c);

    bool rem = slh_remove(ht, djb2("testing"));
    TEST_CHECK(rem == true);
    TEST_CHECK(slh_len(ht) == 0);
    rem = slh_remove(ht, djb2("testing"));
    TEST_CHECK(rem == false);
    teststruct *pitem = slh_get(ht, djb2("testing"));
    TEST_CHECK(pitem == NULL);
    slh_free(ht);

}

void test_overwrite(void)
{
    slh_ctx ht = slh_new(sizeof(int));
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
    TEST_CHECK_(*ret == 7, "val == %d\n", *ret);

    free(ht);

}

TEST_LIST = {
{ "stress test", stress_test_hash },
{"overwrite", test_overwrite},
{"remove test", test_remove},
{"test collision mid", test_collision},
{"test collision edge", test_collision_edge},
{"test custom type", test_custom_type},
{ NULL, NULL }
};