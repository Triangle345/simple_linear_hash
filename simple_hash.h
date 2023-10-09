#pragma once

#include <stdlib.h>

// initial hash array size.
#define SLH_INIT_SIZE 512

// we typedef a custom type to void* to work with our api.
typedef void* slh_ctx;

/**
 * @brief a string haashing algorithm.
 * 
 * @param str 
 * @return size_t 
 */
size_t djb2(unsigned char* str);

/**
 * @brief creates a new simple linear hash table.
 * 
 * @return slh_ctx returns a new hash context that is used throughout the API.
 */
slh_ctx slh_new();

/**
 * @brief destroys and deallocates a given simple linear hash table.
 * 
 * @param my_ht
 */
void slh_free(slh_ctx my_ht);

/**
 * @brief inserts a new entry based on the location of a given key.
 * 
 * @param my_ht 
 * @param key 
 * @param val 
 * @return void* Returns an entry if replaced or null if nothing replaced.
 */
void* slh_insert(slh_ctx my_ht, size_t key, void* val);

/**
 * @brief returns the entry based on the key
 * 
 * @param my_ht 
 * @param key 
 * @return void* Entry associated with the key or NULL if not found.
 */
void* slh_get(slh_ctx my_ht, size_t key);

/**
 * @brief the number of entries in the hash table.
 * 
 * @param my_ht 
 * @return size_t 
 */
size_t slh_len(slh_ctx my_ht);

/**
 * @brief removes a hash entry.
 * 
 * @param my_ht 
 * @param key 
 * @return void* the removed entry.
 */
void* slh_remove(slh_ctx my_ht, size_t key);

// TODO:
// double get_full_ratio(sh_ctx ht);
