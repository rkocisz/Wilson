#ifndef UTIL_H
#define UTIL_H

#include <random>
#include <cinttypes>
#include "enums.h"
#include "board.h"

namespace Util
{
	uint64_t randomU64();
    int popLSB(uint64_t& x);
	int bitCount(uint64_t x);
	uint64_t squareMask(int square);
	Color opposite(Color c);
	std::string squareToNotation(int sq);
	std::string moveToLAN(Move& move);
	Move LANToMove(std::string moveStr, Board& board);

	extern const char* piecesEmotes_[12];
}


#endif
