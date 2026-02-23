#ifndef TESTS_H
#define TESTS_H

#include "board.h"
#include <cassert>
#include <iostream>

void testMoveAndUnmakeMove()
{
    Board board = Board();

    Board boardBackup = board;

    Move move;
    move.captured = PieceType::whiteQueen;
    move.startPos = 9;
    move.endPos = 59;
    move.promotion = PieceType::blackBishop;
    move.moveType = MoveType::normal;
    move.moved = PieceType::blackPawn;

    board.makeMove(move);
    board.unmakeMove(move);

    assert(board == boardBackup);
}

#endif
