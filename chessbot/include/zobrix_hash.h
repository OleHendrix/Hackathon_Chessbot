#ifndef ZOBRIX_HASH_H
#define ZOBRIX_HASH_H

#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

	void store_in_hashmap(uint64_t hash, int score);
	int lookup_in_hashmap(uint64_t hash, int *score);
	uint64_t compute_zobrist_hash(const struct position *pos);
	uint64_t pseudoRandom(uint64_t seed);
#ifdef __cplusplus
}

#endif

#endif
