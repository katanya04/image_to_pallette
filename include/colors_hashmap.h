#ifndef COLORS_HASHMAP_H_INCLUDED
#define COLORS_HASHMAP_H_INCLUDED

#include <stdint.h>
#include <stdbool.h>

int color_compare_3B(const void *entry_1_void, const void *entry_2_void, void *udata);
int color_compare_4B(const void *entry_1_void, const void *entry_2_void, void *udata);

uint64_t color_hash_3B(const void *entry_void, uint64_t seed0, uint64_t seed1);
uint64_t color_hash_4B(const void *entry_void, uint64_t seed0, uint64_t seed1);

bool color_iter_3B(const void *entry_void, void *udata);
bool color_iter_4B(const void *entry_void, void *udata);

#endif