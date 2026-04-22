#ifndef EVALUATION_H
#define EVALUATION_H

#include "board.h"

namespace Eval
{
	int evaluate(Board& board);
	int calculateUpdatedEval(Board& board);
	void updateMaterial(Board& board, const Move& move);
	void scoreMoves(std::vector<Move>& moves, Move ttBestMove);
	void init();
}



#endif // !EVALUATION_H
