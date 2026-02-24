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

void thriceRepetitionTest()
{
    Board board = Board();

    Move move;
    move.captured = PieceType::empty;
    move.startPos = 57;
    move.endPos = 42;
    move.promotion = PieceType::empty;
    move.moveType = MoveType::normal;
    move.moved = PieceType::whiteKnight;
    move.prevCastlingRights = 15;
    move.prevEnPassantSquare = -1;
    move.prevHalfmoveClock = 0;
    move.prevZobristKey = board.getZobristKey();

    Move move2;
    move2.captured = PieceType::empty;
    move2.startPos = 42;
    move2.endPos = 57;
    move2.promotion = PieceType::empty;
    move2.moveType = MoveType::normal;
    move2.moved = PieceType::whiteKnight;
    move2.prevCastlingRights = 15;
    move2.prevEnPassantSquare = -1;
    move2.prevHalfmoveClock = 0;
    move2.prevZobristKey = board.getZobristKey();

    Move move3;
    move3.captured = PieceType::empty;
    move3.startPos = 1;
    move3.endPos = 18;
    move3.promotion = PieceType::empty;
    move3.moveType = MoveType::normal;
    move3.moved = PieceType::blackKnight;
    move3.prevCastlingRights = 15;
    move3.prevEnPassantSquare = -1;
    move3.prevHalfmoveClock = 0;
    move3.prevZobristKey = board.getZobristKey();

    Move move4;
    move4.captured = PieceType::empty;
    move4.startPos = 18;
    move4.endPos = 1;
    move4.promotion = PieceType::empty;
    move4.moveType = MoveType::normal;
    move4.moved = PieceType::blackKnight;
    move4.prevCastlingRights = 15;
    move4.prevEnPassantSquare = -1;
    move4.prevHalfmoveClock = 0;
    move4.prevZobristKey = board.getZobristKey();

    for (int i = 0; i < 3; i++)
    {
        assert(board.getGamestate() == GameState::playing);

        move.prevHalfmoveClock = i * 4; 
        move.prevZobristKey = board.getZobristKey();
        board.makeMove(move);

        move3.prevHalfmoveClock = i * 4 + 1;
        move3.prevZobristKey = board.getZobristKey();
        board.makeMove(move3);

        move2.prevHalfmoveClock = i * 4 + 2;
        move2.prevZobristKey = board.getZobristKey();
        board.makeMove(move2);

        move4.prevHalfmoveClock = i * 4 + 3;
        move4.prevZobristKey = board.getZobristKey();
        board.makeMove(move4);
    }

    assert(board.getGamestate() == GameState::draw);

}

#endif
