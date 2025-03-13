#ifndef MOVE_DEPTH_H
#define MOVE_DEPTH_H

#include <stdint.h>
#include "move.h"

#ifdef __cplusplus
extern "C"
{
#endif

	void setBestMovePerDepth(int depth, struct move mov);
	struct move getBestMovePerDepth(int depth);

#ifdef __cplusplus
}

#endif

#endif
