#include "slh.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
// #include "acutest/include/acutest.h"

#define EMPTY NULL
#define TOMBSTONE -1
#define MAX_FULL .7

#define BUCKET_SEARCH(ht, start) \
        for (size_t i = start+1;i != start; i++) { \
        if (i >= ht->size) { \
            i = 0; \
        } \

typedef struct _bucket  {
    size_t key;
    void* data;
} Bucket;

typedef struct _ctx {
    size_t num_items;
    size_t size;
    Bucket * ht;
} Ctx;

size_t djb2(unsigned char* str) {
    size_t hash = 5381;
    int c;

    while (c = *str++)
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
    
    return hash;
}

slh_ctx slh_new() {
    Ctx* ctx = (Ctx*)calloc(1,sizeof(Ctx));
    ctx->ht = (Bucket*)calloc(SLH_INIT_SIZE, sizeof(Bucket));
    ctx->size = SLH_INIT_SIZE;

    return ctx;
}



static double get_full_ratio(Ctx * my_ht) {
    return (double)my_ht->num_items/(double)my_ht->size;
}

static bool is_bucket_free(Bucket bucket) {
    return ((long long)(bucket.data) == (long long)EMPTY || (long long)(bucket.data) == (long long)TOMBSTONE);
}

static size_t calc_idx(Ctx *my_ht, size_t key) {
    return key % my_ht->size;
}

static size_t find_target_bucket_idx(Ctx* my_ht, size_t key) {
    Bucket* ht = my_ht->ht;
    size_t idx = calc_idx(my_ht, key);
    
    // if we really have the same key, replace it, otherwise check for an empty bucket
    if (ht[idx].key == key) {
        return idx;
    }

    BUCKET_SEARCH(my_ht, idx)
        if (is_bucket_free(ht[i]) || ht[i].key == key) {
            // printf("found free space for idx %ld at %ld\n", idx, i);
            return i;
        }
    }

    printf("error: unable to find free bucket\n");
    return SIZE_MAX;

}

void* slh_insert(slh_ctx my_ht, size_t key, void* val) {
    Ctx* my_ctx = my_ht;
    Bucket* ht = my_ctx->ht; 

    if (get_full_ratio(my_ht) >= MAX_FULL) {
        // save off old list so we may free it afterwards
        Bucket* old_ht = my_ctx->ht;
        size_t old_sz = my_ctx->size;
        // lets double the size
        my_ctx->size *= 2;
        my_ctx->ht = (Bucket*)calloc(my_ctx->size, sizeof(Bucket));
        my_ctx->num_items = 0;

        for (int i = 0; i < old_sz; i++) {
            if (!is_bucket_free(old_ht[i])) {
                slh_insert(my_ht, old_ht[i].key, old_ht[i].data);
            }
        }
        
        free(old_ht);
        ht = my_ctx->ht;
    }

    size_t idx = calc_idx(my_ht, key);
    if (!is_bucket_free(ht[idx])) {
        //TODO: need to not only find free bucket but to replace one that has same key
        idx = find_target_bucket_idx(my_ht, key);
    }

    // lets determine if we are replacing instead of adding new. If so,
    // we do not want to increment number of items. We also want to return 
    // the old value in case it needs to be freed
    void* savedDat = NULL;
    if (is_bucket_free(ht[idx])) {
        my_ctx->num_items++;
    } else if (ht[idx].key == key) {
        savedDat = ht[idx].data;
    }

    ht[idx].data = val;
    ht[idx].key = key;

    return savedDat;
}

size_t slh_len(slh_ctx my_ht) {
    Ctx* _ht = my_ht;
    return _ht->num_items;
}

void* slh_get(slh_ctx my_ht, size_t key) {
    Ctx* _ht = my_ht;

    size_t idx = calc_idx(my_ht, key);

    if (is_bucket_free(_ht->ht[idx])) {
        return NULL;
    }

    idx = find_target_bucket_idx(my_ht, key);


    if (is_bucket_free(_ht->ht[idx])) {
        return NULL;
    }

    return _ht->ht[idx].data;
}

void* slh_remove(slh_ctx my_ht, size_t key) {
    Ctx* my_ctx = my_ht;
    Bucket* ht = my_ctx->ht;

    size_t idx = find_target_bucket_idx(my_ht, key);
    
    void* ret = NULL;

    // if we cant find the target bucket, return NULL
    if (idx == SIZE_MAX || is_bucket_free(ht[idx])) {
        return ret;
    }

    // lets save off the main idx we are looking for. This will assist us
    // in fixing the fragmenting buckets later on
    size_t orig_idx = calc_idx(my_ht, key);

    ret = ht[idx].data;
    ht[idx].data = (void*)TOMBSTONE;
    ht[idx].key = 0;
    
    //while not == empty move buckets to left to fill in empty
    BUCKET_SEARCH(my_ctx, idx) 
        if (is_bucket_free(ht[i])) {
            break;
        } 
        
        // we account for wraparound
        size_t i_prev = i;
        if(i == 0) {
            i_prev = my_ctx->size - 1;
        } else {
            i_prev = i-1;
        } 

        // if we found a bucket that collided, we move it down to fill the empty spot
        // keep doing this until we hit an empty bucket 
        if (calc_idx(my_ctx ,ht[i].key) == orig_idx) {
            ht[i_prev].data = ht[i].data;
            ht[i_prev].key = ht[i].key;
            ht[i].data = (void*)TOMBSTONE;
            ht[i].key = 0;
        }
    }

    my_ctx->num_items--;
    return ret;
}

void slh_free(slh_ctx my_ht) {
    Ctx* my_ctx = my_ht;
    if (my_ctx) {
        if (my_ctx->ht) {
            free(my_ctx->ht);
        }

        free(my_ht);
    }
}
