#ifndef MOVEGENUTIL_H
#define MOVEGENUTIL_H

#include <cinttypes>
#include "enums.h"

namespace MoveGenUtil
{
	void init();

	uint64_t computeBishopMoves(int square, uint64_t occupancy);
	uint64_t computeRookMoves(int square, uint64_t occupancy);
	uint64_t getBishopMoves(int square, uint64_t occupancy);
	uint64_t getRookMoves(int square, uint64_t occupancy);

	extern uint64_t knightMoves_[64];
	extern uint64_t kingMoves_[64];
}

#endif // !MOVEGENUTIL_H
