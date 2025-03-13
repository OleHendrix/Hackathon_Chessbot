#include "search.h"
#include "evaluate.h"
#include "generate.h"
#include "types.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <move.h>

#include <limits.h>

size_t generate_pseudo_legal_moves_per_piece_square(const struct position *pos, struct move *moves, int square)
{
	size_t count = 0;
	int forward = pos->side_to_move == WHITE ? 1 : -1;
	int piece = pos->board[square];

	switch (TYPE(piece))
	{
		int up;
		int up_up;

	case PAWN:
		up = add_offset(square, 0, forward);
		up_up = add_offset(square, 0, forward * 2);

		/* pawn push.                                                    */
		if (up != NO_SQUARE && pos->board[up] == NO_PIECE)
		{
			count += generate_pawn_move(pos, moves + count, square, up);

			/* double pawn push.                                         */
			if (up_up != NO_SQUARE && pos->board[up_up] == NO_PIECE)
			{
				if (RANK(square) == RELATIVE(RANK_2, COLOR(piece)))
				{
					count += generate_pawn_move(pos, moves + count, square, up_up);
				}
			}
		}

		/* pawn captures.                                                */
		count += generate_pawn_capture(pos, moves + count, square, -1, forward);
		count += generate_pawn_capture(pos, moves + count, square, 1, forward);

		break;
	case KNIGHT:
		/* knight moves.                                                 */
		count += generate_simple_move(pos, moves + count, square, -1, -2);
		count += generate_simple_move(pos, moves + count, square, 1, -2);
		count += generate_simple_move(pos, moves + count, square, -2, -1);
		count += generate_simple_move(pos, moves + count, square, 2, -1);
		count += generate_simple_move(pos, moves + count, square, -2, 1);
		count += generate_simple_move(pos, moves + count, square, 2, 1);
		count += generate_simple_move(pos, moves + count, square, -1, 2);
		count += generate_simple_move(pos, moves + count, square, 1, 2);

		break;
	case BISHOP:
	case ROOK:
	case QUEEN:
		/* bishop and queen moves.                                       */
		if (TYPE(piece) != ROOK)
		{
			count += generate_sliding_move(pos, moves + count, square, -1, -1);
			count += generate_sliding_move(pos, moves + count, square, 1, -1);
			count += generate_sliding_move(pos, moves + count, square, -1, 1);
			count += generate_sliding_move(pos, moves + count, square, 1, 1);
		}

		/* rook and queen moves.                                         */
		if (TYPE(piece) != BISHOP)
		{
			count += generate_sliding_move(pos, moves + count, square, 0, -1);
			count += generate_sliding_move(pos, moves + count, square, -1, 0);
			count += generate_sliding_move(pos, moves + count, square, 1, 0);
			count += generate_sliding_move(pos, moves + count, square, 0, 1);
		}

		break;
	case KING:
		/* simple king moves.                                            */
		count += generate_simple_move(pos, moves + count, square, -1, -1);
		count += generate_simple_move(pos, moves + count, square, 0, -1);
		count += generate_simple_move(pos, moves + count, square, 1, -1);
		count += generate_simple_move(pos, moves + count, square, -1, 0);
		count += generate_simple_move(pos, moves + count, square, 1, 0);
		count += generate_simple_move(pos, moves + count, square, -1, 1);
		count += generate_simple_move(pos, moves + count, square, 0, 1);
		count += generate_simple_move(pos, moves + count, square, 1, 1);

		/* king side castling.                                           */
		if (pos->castling_rights[pos->side_to_move] & KING_SIDE)
		{
			int f1 = SQUARE(FILE_F, RELATIVE(RANK_1, pos->side_to_move));
			int g1 = SQUARE(FILE_G, RELATIVE(RANK_1, pos->side_to_move));
			int f1_empty = pos->board[f1] == NO_PIECE;
			int g1_empty = pos->board[g1] == NO_PIECE;

			if (f1_empty && g1_empty)
			{
				moves[count++] = make_move(square, g1, NO_TYPE);
			}
		}

		/* queen side castling.                                          */
		if (pos->castling_rights[pos->side_to_move] & QUEEN_SIDE)
		{
			int b1 = SQUARE(FILE_B, RELATIVE(RANK_1, pos->side_to_move));
			int c1 = SQUARE(FILE_C, RELATIVE(RANK_1, pos->side_to_move));
			int d1 = SQUARE(FILE_D, RELATIVE(RANK_1, pos->side_to_move));
			int b1_empty = pos->board[b1] == NO_PIECE;
			int c1_empty = pos->board[c1] == NO_PIECE;
			int d1_empty = pos->board[d1] == NO_PIECE;

			if (b1_empty && c1_empty && d1_empty)
			{
				moves[count++] = make_move(square, c1, NO_TYPE);
			}
		}

		break;
	}
	return count;
}

size_t generate_legal_moves_per_piece_square(const struct position *pos, int moveFromSquare)
{
	struct move moves[MAX_MOVES];

	size_t pseudo_legal_count = generate_pseudo_legal_moves_per_piece_square(pos, moves, moveFromSquare);
	size_t index;
	size_t count = 0;

	for (index = 0; index < pseudo_legal_count; index++)
	{
		if (is_legal(pos, moves[index]))
		{
			moves[count++] = moves[index];
		}
	}

	return count;
}

static const int piece_value[6] = { 100, 300, 300, 500, 900, 1000000 };

int compareMoveScore(const void *a, const void *b)
{
	struct move *move_a = (struct move *)a;
	struct move *move_b = (struct move *)b;

	return (move_b->moveScore - move_a->moveScore);
}

void moveOrdering(const struct position *pos, struct move moves[], size_t count)
{
	for (size_t i = 0; i < count; i ++)
	{
		int moveToSquare = moves[i].to_square;
		int moveFromSquare = moves[i].from_square;
		// int pieceType = pos->board[moveFromSquare];
		int pieceValue = piece_value[pos->board[moveFromSquare]];

		//VOORDELEN

		//andere steen wegspelen in verhouding met welke steen je dat doet
		int captured = pos->board[moveToSquare];
		if (captured != NO_PIECE) //check of dit klopt
			moves[i].moveScore += 10 * (piece_value[captured] - pieceValue);

		//of een pion de achterrij raakt en kan promoveren en naar welk type dit gebeurt
		int promotionType = moves[i].promotion_type;
		if (promotionType) //check of dit klopt
			moves[i].moveScore += piece_value[promotionType];

		//of je het centrum ermee bezet
		if ((FILE(moveToSquare) == 3 || FILE(moveToSquare) == 4) && (RANK(moveToSquare) == 3 || RANK(moveToSquare) == 4))
			moves[i].moveScore += 5 * pieceValue;

		int availableMoveDifference = (int)(generate_legal_moves_per_piece_square(pos, moveToSquare) - generate_legal_moves_per_piece_square(pos, moveFromSquare));
			moves[i].moveScore += (2 * availableMoveDifference);
		//NADELEN
	}
	qsort(moves, count, sizeof(struct move), compareMoveScore);
}

int searchCaptures(const struct position *pos, struct search_info *info, int alpha, int beta, bool maximizingPlayer)
{
	int evaluation = evaluate(pos, info);
	if (evaluation >= beta)
		return beta;
	if (evaluation > alpha)
		alpha = evaluation;

	struct move moves[MAX_MOVES];
	size_t count = generate_legal_moves(pos, moves);
	moveOrdering(pos, moves, count);

	for (size_t index = 0; index < count; index++)
	{
		if (pos->board[moves[index].to_square] == NO_PIECE || COLOR(pos->board[moves[index].to_square]) == pos->side_to_move)
			continue;

		struct position copy = *pos;
		int score;
		do_move(&copy, moves[index]);

		if (maximizingPlayer)
		{
			score = searchCaptures(&copy, info, alpha, beta, false);
			if (score > alpha)
				alpha = score;
		}
		else if (!maximizingPlayer)
		{
			score = searchCaptures(&copy, info, alpha, beta, true);
			if (score < beta)
				beta = score;
		}
		if (alpha >= beta)
			break;
		}
		return maximizingPlayer ? alpha : beta;
}

struct search_result minimax(const struct position *pos, int depth, int alpha, int beta, bool maximizingPlayer, struct search_info *info)
{
	struct search_result result;

	result.score = maximizingPlayer ? -1000000 : 1000000;

	if (depth == 0)
	{
		result.score = evaluate(pos, info);
		if (pos->side_to_move == BLACK)
			result.score = -result.score;
		result.score = searchCaptures(pos, info, alpha, beta, info->pos->side_to_move);
		return result;
	}
	struct move moves[MAX_MOVES];
	size_t count = generate_legal_moves(pos, moves);
	// moveOrdering(pos, moves, count);

	for (size_t index = 0; index < count; index++)
	{
		struct position copy = *pos;
		int score;
		do_move(&copy, moves[index]);

		if (maximizingPlayer)
		{
			score = minimax(&copy, depth - 1, alpha, beta, false, info).score;
			if (score > result.score)
			{
				result.move = moves[index];
				result.score = score;
			}
			if (score > alpha)
				alpha = score;
		}
		else if (!maximizingPlayer)
		{
			score = minimax(&copy, depth - 1, alpha, beta, true, info).score;
			if (score < result.score)
			{
				result.move = moves[index];
				result.score = score;
			}
			if (score < beta)
				beta = score;
		}
		if (alpha >= beta)
			break;
	}
	return result;
}

struct move search(struct search_info *info)
{
	return minimax(info->pos, 4, -1000000, 1000000, info->pos->side_to_move, info).move;
}
