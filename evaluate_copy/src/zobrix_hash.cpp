#include "zobrix_hash.h"
#include <unordered_map> 
#include <cstdint>

std::unordered_map<uint64_t, int> zobrix_hash;

uint64_t pseudoRandom(uint64_t seed)
{
	seed ^= seed >> 21;
	seed ^= seed << 35;
	seed ^= seed >> 4;
	seed *= 2685821657736338717ULL;
	return seed;
}

void store_in_hashmap(uint64_t hash, int score)
{
	zobrix_hash[hash] = score;
}

int lookup_in_hashmap(uint64_t hash, int *score)
{
	auto it = zobrix_hash.find(hash);
	if (it != zobrix_hash.end())
	{
		*score = it->second;
		return 1;
	}
	return 0;
}