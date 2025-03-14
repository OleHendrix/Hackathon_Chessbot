#include "search.h"
#include "evaluate.h"
#include "generate.h"
#include "types.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <move.h>

#include <limits.h>
#include <time.h>

static const int piece_value[6] = { 100, 320, 330, 500, 900, 1000000 };

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
		int pieceValue = piece_value[pos->board[moveFromSquare]];

		// If you capture a piece of enemy, in factor with that kind you piece you do that.
		int captured = pos->board[moveToSquare];
		if (captured != NO_PIECE) //check of dit klopt
			moves[i].moveScore += 10 * (piece_value[captured] - pieceValue);

		// If pawn can have the ablity to promote.
		int promotionType = moves[i].promotion_type;
		if (promotionType) //check of dit klopt
			moves[i].moveScore += piece_value[promotionType];

		// If the move gets you more towards the centre.
		if ((FILE(moveToSquare) == 3 || FILE(moveToSquare) == 4) && (RANK(moveToSquare) == 3 || RANK(moveToSquare) == 4))
			moves[i].moveScore += 5 * pieceValue;
	}
	qsort(moves, count, sizeof(struct move), compareMoveScore);
}
	
struct search_result minimax(const struct position *pos, int depth, int alpha, int beta, struct search_info *info, double timeForSearch, int startTime, struct move bestMoves[])
{
	struct search_result result;
	
	result.score = pos->side_to_move == WHITE ? -1000000 : 1000000;

	int currentTime = clock();

	if ((double)(currentTime - startTime) / CLOCKS_PER_SEC >= timeForSearch)
	{
		result.score = 0;
		return result;
	}
	if (depth == 0)
	{
		result.score = evaluate(pos, info);
		if (pos->side_to_move == BLACK)
			result.score = -result.score;
		return result;
	}
	struct move moves[MAX_MOVES];
	size_t count = generate_legal_moves(pos, moves);
	moveOrdering(pos, moves, count);
		
	struct move bestPrevMove = bestMoves[depth];
	if (bestPrevMove.from_square != 0 || bestPrevMove.to_square != 0)
	{
		for (size_t index = 0; index < count; index++)
		{
			if (moves[index].from_square == bestPrevMove.from_square &&
				moves[index].to_square == bestPrevMove.to_square)
			{
				struct move temp = moves[0];
				moves[0] = moves[index];
				moves[index] = temp;
				break;
			}
		}
	}
	for (size_t index = 0; index < count; index++)
	{
		struct position copy = *pos;
		int score;
		do_move(&copy, moves[index]);

		if (pos->side_to_move == WHITE)
		{
			score = minimax(&copy, depth - 1, alpha, beta, info, timeForSearch, startTime, bestMoves).score;
			if (score > result.score)
			{
				result.move = moves[index];
				result.score = score;
			}
			if (score > alpha)
				alpha = score;
		}
		else
		{
			score = minimax(&copy, depth - 1, alpha, beta, info, timeForSearch, startTime, bestMoves).score;
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

double getTimeForSearch(struct search_info *info, struct position *pos)
{
	double timeRatio = (double)info->time[info->pos->side_to_move] / (double)pos->maxTimePerSide;
	FILE *log_file = fopen("SEARCHLOGS.txt", "a");
	if (log_file)
		fprintf(log_file, "timeRatio %f\n", timeRatio);

	// Begingame: give less time for search, quick plays.
	if (timeRatio > 0.9)
	{
		fprintf(log_file, "Begingame\n");
		fclose(log_file);
		return (info->time[info->pos->side_to_move] / 70000.0);
	}

	// Midgame, give more time for search, deeper thinking.
	if (timeRatio > 0.3)
	{
		fprintf(log_file, "Midgame\n");
		fclose(log_file);
		return (info->time[info->pos->side_to_move] / 10000.0);
	}

	// Endgame, not a lot of time left, quick moves.
	if (timeRatio < 0.3)
	{
		fprintf(log_file, "Endgame\n");
		fclose(log_file);
		return (info->time[info->pos->side_to_move] / 70000.0);
	}
	// Fallback
	fclose(log_file);
	return (info->time[info->pos->side_to_move] / 50000.0);
}

struct move search(struct search_info *info, struct position *pos)
{
	struct move bestMove;
	int maxDepth = 10;
	double timeForSearch = getTimeForSearch(info, pos);

	FILE *log_file = fopen("SEARCHLOGS.txt", "a");
	if (log_file)
	{
		fprintf(log_file, "Time available for search: %f\n", timeForSearch);
		fclose(log_file);
	}
	struct move movePerDepth[maxDepth];
	int startTime = clock();
	for (int i = 1; i < maxDepth; i ++)
	{
		struct search_result res = minimax(info->pos, i, -1000000, 1000000, info, timeForSearch, startTime, movePerDepth);
		int currentTime = clock();
		if ((double)(currentTime - startTime) / CLOCKS_PER_SEC >= timeForSearch)
		{
			FILE *log_file = fopen("SEARCHLOGS.txt", "a");
			if (log_file)
			{
				fprintf(log_file, "Best move used in depth: %d\n", i);
				fprintf(log_file, "Positions evaluated: %llu\n\n", info->evaluatedPositions);
				fclose(log_file);
			}
			break;
		}
		bestMove = res.move;
		movePerDepth[i] = bestMove;
	}
	return bestMove;
}
