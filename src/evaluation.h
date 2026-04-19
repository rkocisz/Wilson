#ifndef EVALUATION_H
#define EVALUATION_H

#include "board.h"

namespace Eval
{
	int evaluate(const Board& board);
	int updateEval(Board& board, const Move& move);
	void scoreMoves(std::vector<Move>& moves, Move ttBestMove);
	void init();
}



#endif // !EVALUATION_H
