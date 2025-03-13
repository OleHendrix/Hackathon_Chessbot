#include "zobrix_hash.h"
#include <unordered_map> 
#include <cstdint>

uint64_t zobrist_pieces[12][64];	// [Piece type][Square]
uint64_t zobrist_castling_white[4]; // 16 possible castling rights
uint64_t zobrist_castling_black[4];
uint64_t zobrist_en_passant[8]; // 8 possible en passant files
uint64_t zobrist_side;

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