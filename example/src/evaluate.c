#include "evaluate.h"
#include "types.h"
#include "search.h"
#include "heatmaps.h"
#include "zobrix_hash.h"

static const int piece_value[6] = { 100, 300, 300, 500, 900, 1000000 };

int evaluate(const struct position *pos, struct search_info *info)
{
	int score[2] = { 0, 0 };
	int square;

	// u_int64_t hash = compute_zobrist_hash(pos);
	// int storedscore;
	// if (lookup_in_hashmap(hash, &storedscore))
	// 	return storedscore;

	for (square = 0; square < 64; square++)
	{
		int piece = pos->board[square];

		if (piece != NO_PIECE)
		{
			score[COLOR(piece)] += piece_value[TYPE(piece)];
			score[COLOR(piece)] += check_heatmap(piece, COLOR(piece) == BLACK ? 63 - square : square); 
		}
	}

	int evaluation = score[pos->side_to_move] - score[1 - pos->side_to_move];
	// store_in_hashmap(hash, evaluation);

	info->evaluatedPositions++;
	return evaluation;
}
