#ifndef MOVEGEN_H
#define MOVEGEN_H

#include "board.h"

namespace MoveGen
{
	std::vector<Move> generateLegalMoves(Board* board);
	bool isInCheck(Board* board);
}

#endif 
