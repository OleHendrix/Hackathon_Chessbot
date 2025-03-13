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

		if (piece != NO_PIECE) // Als er een stuk staat
		{
			hash ^= pos->zobrist_table[piece][square]; 
		}
	}

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

	info->evaluatedPositions++;

	int evaluation = score[pos->side_to_move] - score[1 - pos->side_to_move];
	store_in_hashmap(hash, evaluation);

	info->evaluatedPositions++;
	return evaluation;
}

// int moveToSquare = moves[i].to_square;
// int moveFromSquare = moves[i].from_square;
// // int pieceType = pos->board[moveFromSquare];
// int pieceValue = piece_value[pos->board[moveFromSquare]];

//VOORDELEN

// //andere steen wegspelen in verhouding met welke steen je dat doet
// int captured = pos->board[moveToSquare];
// if (captured != NO_PIECE) //check of dit klopt
// 	moves[i].moveScore += 10 * (piece_value[captured] - pieceValue);

// //of een pion de achterrij raakt en kan promoveren en naar welk type dit gebeurt
// int promotionType = moves[i].promotion_type;
// if (promotionType) //check of dit klopt
// 	moves[i].moveScore += piece_value[promotionType];

// //of je het centrum ermee bezet
// if ((FILE(moveToSquare) == 3 || FILE(moveToSquare) == 4) && (RANK(moveToSquare) == 3 || RANK(moveToSquare) == 4))
// 	moves[i].moveScore += 5 * pieceValue;

// int availableMoveDifference = (int)(generate_legal_moves_per_piece_square(pos, moveToSquare) - generate_legal_moves_per_piece_square(pos, moveFromSquare));
// moves[i].moveScore += (5 * availableMoveDifference);