#ifndef ZOBRIX_HASH_H
#define ZOBRIX_HASH_H

#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

	void store_in_hashmap(uint64_t hash, int score);
	int lookup_in_hashmap(uint64_t hash, int *score);
	void print_hash(void);
	uint64_t compute_zobrist_hash(const struct position *pos);

	struct book_entry
	{
		uint64_t key;	 // Zobrist hash of position (8 bytes)
		uint16_t move;	 // Encoded move (2 bytes)
		uint16_t weight; // Frequency (2 bytes)
		uint32_t learn;	 // Learning value (4 bytes)
	};

#ifdef __cplusplus
}
#endif

#endif // ZOBRIX_HASH_H
