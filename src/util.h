#ifndef UTIL_H
#define UTIL_H

#include <random>
#include <cinttypes>
#include "enums.h"

namespace Util
{
	uint64_t randomU64();
    int popLSB(uint64_t& x);
	int bitCount(uint64_t x);
	uint64_t squareMask(int square);
	Color opposite(Color c);

	void initKnightMoves();
	void initKingMoves();
	void initMagicBitboards();

	uint64_t computeBishopMoves(int square, uint64_t occupancy);
	uint64_t computeRookMoves(int square, uint64_t occupancy);
	uint64_t computeRookRelevantOccupancy(int square);

	uint64_t generateBishopOccupancyFromIndex(int square, int index);
	uint64_t generateRookOccupancyFromIndex(int square, int index);

	extern uint64_t knightMoves_[64];
	extern uint64_t kingMoves_[64];
	extern const char* piecesEmotes_[12];
}


#endif
