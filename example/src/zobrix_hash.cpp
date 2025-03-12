#include "zobrix_hash.h"
#include <unordered_map> 
#include <cstdint>  

struct hash_score
{
    int score;
    int depth;
};

std::unordered_map<uint64_t, hash_score> zobrix_hash;

void store_in_hashmap(uint64_t hash, int score, int depth)
{
    zobrix_hash[hash] = {score, depth};
}

int lookup_in_hashmap(uint64_t hash, int *score, int *depth)
{
	auto it = zobrix_hash.find(hash);
	if (it != zobrix_hash.end())
	{
		*score = it->second.score;
        *depth = it->second.depth;
        return 1;
	}
	return 0;
}
