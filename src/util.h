#ifndef UTIL_H
#define UTIL_H

#include <random>
#include <cinttypes>

namespace Util
{
	uint64_t randomU64();
    int popLSB(uint64_t& bb);
	void initKnightMoves();
	void initKingMoves();
	uint64_t squareMask(int square);


	extern uint64_t knightMoves_[64];
	extern uint64_t kingMoves_[64];
}


#endif
