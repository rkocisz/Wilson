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
    move.prevCastlingRights = 15;
    move.prevEnPassantSquare = -1;
    move.prevHalfmoveClock = 0;
    move.prevZobristKey = board.getZobristKey();

    board.makeMove(move);
    board.unmakeMove(move);

    assert(board == boardBackup);
}

void testZobristKeyGeneration()
{
    Board board = Board();

    Move move;
    move.captured = PieceType::whiteQueen;
    move.startPos = 9;
    move.endPos = 59;
    move.promotion = PieceType::blackBishop;
    move.moveType = MoveType::normal;
    move.moved = PieceType::blackPawn;
    move.prevCastlingRights = 15;
    move.prevEnPassantSquare = -1;
    move.prevHalfmoveClock = 0;
    move.prevZobristKey = board.getZobristKey();

    board.makeMove(move);

    assert(board.getZobristKey() == board.getNewlyGeneratedZobristKey());
}

#endif
