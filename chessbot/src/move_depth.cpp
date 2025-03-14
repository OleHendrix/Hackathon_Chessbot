#include <unordered_map>
#include "move.h"
#include <cstdint>

std::unordered_map<int, struct move> bestMovePerDepth;

void setBestMovePerDepth(int depth, struct move mov)
{
	bestMovePerDepth[depth] = mov;
}

struct move getBestMovePerDepth(int depth)
{
	return (bestMovePerDepth.find(depth)->second);
}