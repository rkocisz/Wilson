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
	uint8_t prevCastlingRights;
	int prevEnPassantSquare;
	int prevHalfmoveClock;
};

#endif 
