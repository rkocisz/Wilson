#ifndef BOARD_H
#define BOARD_H

#include <vector>
#include <string>
#include <cinttypes>
#include "enums.h"
#include "structs.h"


class Board
{
public:
	bool operator==(const Board&) const = default;

	Board();
	void draw();
	void makeMove(Move move);
	void unmakeMove(Move move);
	void loadStartPos();

private:

inline uint64_t squareMask(int square);	

PieceType board_[64];
uint64_t bitBoards_[12];

uint8_t castlingRights_;
};

#endif