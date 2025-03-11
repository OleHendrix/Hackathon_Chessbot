#include "search.h"
#include "evaluate.h"
#include "generate.h"

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <stdint.h>

static const int piece_value[6] = { 100, 300, 300, 500, 900, 1000000 };

int compare_moves(const void *a, const void *b) {
    struct move *move_a = (struct move *)a;
    struct move *move_b = (struct move *)b;

    // Return the result of comparing movescore in descending order
    return move_b->moveScore - move_a->moveScore;  // Reverse the order for descending
}

void moveOrdering(const struct position *pos, struct move moves[], size_t count)
{
    // int moveScores[MAX_MOVES];

    for (size_t i = 0; i < count; i ++)
    {
        int moveToSquare = moves[i].to_square;
        int moveFromSquare = moves[i].from_square;
        int pieceValue = piece_value[pos->board[moveFromSquare]];


        //VOORDELEN
        // if (moveToSquare == 13)
        //     moves[i].moveScore = 1000;
        //andere steen wegspelen in verhouding met welke steen je dat doet
        int captured = pos->board[moveToSquare];
        if (captured) //check of dit klopt
            moves[i].moveScore += 10 * (piece_value[captured] - pieceValue);

        //of een pion de achterrij raakt en kan promoveren en naar welk type dit gebeurt
        int promotionType = moves[i].promotion_type;
        if (promotionType) //check of dit klopt
            moves[i].moveScore += piece_value[promotionType];

        //of je het centrum ermee bezet
        // if ((FILE(moveToSquare) == 3 || FILE(moveToSquare) == 4) && (RANK(moveToSquare) == 3 || RANK(moveToSquare) == 4))
        //     moveScore += 5 * pieceValue;
        //NADELEN
    }
    qsort(moves, count, sizeof(struct move), compare_moves);
}

struct search_result minimax(const struct position *pos, int depth, int alpha, int beta, int maximizingPlayer)
{
    struct search_result result;

    result.score = maximizingPlayer ? -1000000 : 1000000;
    
    if (depth == 0)
    {
        result.score = evaluate(pos);
        return result;
    }
    struct move moves[MAX_MOVES];
    size_t count = generate_legal_moves(pos, moves);
    moveOrdering(pos, moves, count);
    
    for (size_t index = 0; index < count; index++)
    {
        struct position copy = *pos;
        int score;
        do_move(&copy, moves[index]);
        
        if (maximizingPlayer)
        {
            score = minimax(&copy, depth - 1, alpha, beta, 0).score;
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
            score = minimax(&copy, depth - 1, alpha, beta, 1).score;
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


struct move search(const struct search_info *info)
{
    // struct move book_move = {0};
    
    // find_book_move(info, &book_move);

    // if (book_move.from_square != 0 || book_move.to_square != 0) {
    //     return book_move;
    // }
    return minimax(info->pos, 6, -1000000, 1000000, info->pos->side_to_move).move;
}
