#include "evaluate.h"
#include "types.h"
#include "search.h"
#include "heatmaps.h"
#include "zobrix_hash.h"
#include "position.h"

static const int piece_value[6] = { 100, 320, 330, 500, 900, 1000000 };

uint64_t compute_zobrist_hash(const struct position *pos)
{
	uint64_t hash = 0;

	for (int square = 0; square < 64; square++)
	{
		int piece = pos->board[square];
		if (piece != NO_PIECE)
		{
			hash ^= pos->zobrist_pieces[piece][square];
		}
	}

	if (pos->side_to_move == WHITE)
		hash ^= pos->zobrist_side;

	hash ^= pos->zobrist_castling_white[pos->castling_rights[WHITE]];
	hash ^= pos->zobrist_castling_black[pos->castling_rights[BLACK]];

	if (pos->en_passant_square != NO_SQUARE)
		hash ^= pos->zobrist_en_passant[pos->en_passant_square % 8];

	return hash;
}

int check_heatmap(int pieceType, int square)
{
	switch (pieceType)
	{
		case PAWN:
			return (pawn_table[square]);
		case KNIGHT:
			return (knight_table[square]);
		case BISHOP:
			return (bishop_table[square]);
		case ROOK:
			return (rook_table[square]);
		case QUEEN:
			return (queen_table[square]);
		case KING:
			return (king_table[square]);
	}
	return 0;
}

int evaluate(const struct position *pos, struct search_info *info)
{
	int score[2] = { 0, 0 };

	uint64_t hash = compute_zobrist_hash(pos);
	int storedscore;
	if (lookup_in_hashmap(hash, &storedscore))
		return storedscore;

	for (int square = 0; square < 64; square++) {
		int piece = pos->board[square];

		if (piece != NO_PIECE)
		{
			score[COLOR(piece)] += piece_value[TYPE(piece)];
			score[COLOR(piece)] += check_heatmap(TYPE(piece), COLOR(piece) == BLACK ? square : square ^ 56 );
		}
	}

	int evaluation = score[pos->side_to_move] - score[1 - pos->side_to_move];
	store_in_hashmap(hash, evaluation);

	info->evaluatedPositions++;
	return evaluation;
}

int evaluateTotal(const struct position *pos)
{
	int score = 0;
	for (int square = 0; square < 64; square++)
	{
		int piece = pos->board[square];

		if (piece != NO_PIECE)
			score += piece_value[TYPE(piece)];
	}
	return score;
}
