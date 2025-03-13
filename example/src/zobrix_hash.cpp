#include "zobrix_hash.h"
#include "position.h"
#include "types.h"

#include <unordered_map> 
#include <cstdint>
#include <iostream> 

std::unordered_map<uint64_t, int> zobrix_hash;

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

void print_hash(void)
{
	for (auto it = zobrix_hash.begin(); it != zobrix_hash.end(); it ++)
		std::cout << it->second << std::endl;
}

int has_pawn_for_capture(const struct position *pos)
{
	int sqWithPawn = 0;
	int whitePawn = PIECE(WHITE, PAWN); // Using your PIECE macro
	int blackPawn = PIECE(BLACK, PAWN);
	int pawn = pos->side_to_move == WHITE ? whitePawn : blackPawn;

	if (pos->en_passant_square != NO_SQUARE)
	{
		// Calculate the square of the pawn that just moved two squares
		if (pos->side_to_move == WHITE)
		{
			sqWithPawn = pos->en_passant_square - 8;
		}
		else
		{
			sqWithPawn = pos->en_passant_square + 8;
		}

		int file = FILE(pos->en_passant_square);

		// Check if there are pawns to the left or right that could capture
		if (file > 0 && pos->board[sqWithPawn - 1] == pawn)
		{
			return 1; // TRUE
		}
		if (file < 7 && pos->board[sqWithPawn + 1] == pawn)
		{
			return 1; // TRUE
		}
	}

	return 0; // FALSE
}

uint64_t compute_zobrist_hash(const struct position *pos)
{
	int sq = 0, file = 0;
	uint64_t finalKey = 0;
	int piece = NO_PIECE;

	// Loop through all squares on the board
	for (sq = 0; sq < 64; ++sq)
	{
		piece = pos->board[sq];
		// Check if square has a valid piece
		if (piece != NO_PIECE)
		{
			finalKey ^= pos->zobristTable[piece][sq];
		}
	}

	// Side to move
	if (pos->side_to_move == BLACK)
	{
		finalKey ^= pos->blackToMove;
	}

	// Castling rights
	if (pos->castling_rights[WHITE] & KING_SIDE)
		finalKey ^= pos->castlingRights[0]; // White kingside
	if (pos->castling_rights[WHITE] & QUEEN_SIDE)
		finalKey ^= pos->castlingRights[1]; // White queenside
	if (pos->castling_rights[BLACK] & KING_SIDE)
		finalKey ^= pos->castlingRights[2]; // Black kingside
	if (pos->castling_rights[BLACK] & QUEEN_SIDE)
		finalKey ^= pos->castlingRights[3]; // Black queenside

	// En passant
	if (pos->en_passant_square != NO_SQUARE)
	{
		if (has_pawn_for_capture(pos))
		{
			file = FILE(pos->en_passant_square);
			finalKey ^= pos->enPassantFile[file];
		}
	}

	return finalKey;
}
