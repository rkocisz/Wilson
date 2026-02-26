#ifndef STRUCTS_H
#define STRUCTS_H

#include "enums.h"
#include <cinttypes>

struct Move
{
	uint64_t startPos;
	uint64_t endPos;
	PieceType moved;
	PieceType captured;
	PieceType promotion;
	MoveType moveType;

	bool operator==(const Move&) const = default; 

	Move(uint64_t startPos, uint64_t endPos, PieceType moved, PieceType captured, PieceType promotion, MoveType moveType)
	: startPos(startPos)
	, endPos(endPos)
	, moved(moved)
	, captured(captured)
	, promotion(promotion)
	, moveType(moveType)
	{ 
	}
};

struct UnmakeInfo 
{
	uint8_t prevCastlingRights;
	int prevEnPassantSquare;
	int prevHalfmoveClock;
	uint64_t prevZobristKey;

	bool operator==(const UnmakeInfo&) const = default;
};

#endif 
