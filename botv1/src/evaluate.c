#include "evaluate.h"
#include "types.h"

static const int piece_value[6] = { 100, 300, 300, 500, 900, 1000000 };

int count_material(const struct position *pos)
{
	int score[2] = { 0, 0 };
	int square;

	for (square = 0; square < 64; square++) {
		int piece = pos->board[square];

		if (piece != NO_PIECE) {
			score[COLOR(piece)] += piece_value[TYPE(piece)];
		}
	}

	return score[pos->side_to_move] - score[1 - pos->side_to_move];
}

int evaluate(const struct position *pos) {
	int score = 0;
	score += count_material(pos);
	for (int square = 0; square < 64; square++) {
        if (pos->board[square] == KNIGHT) {
            if (square == 21) {  // Assuming E3 is correctly defined in your board representation
                score += 10000; // Assign a high priority to this position
            }
        }
	}
	return score;
}
