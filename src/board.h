#ifndef BOARD_H
#define BOARD_H


#include <cinttypes>
class Board
{
public:

	Board();
	void draw();

private:

uint64_t whiteKing_;
uint64_t whiteQueen_;
uint64_t whiteRook_;
uint64_t whiteKnight_;
uint64_t whiteBishop_;
uint64_t whitePawn_;

uint64_t blackKing_;
uint64_t blackQueen_;
uint64_t blackRook_;
uint64_t blackKnight_;
uint64_t blackBishop_;
uint64_t blackPawn_;

};

#endif