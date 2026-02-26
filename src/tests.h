#ifndef TESTS_H
#define TESTS_H

#include "board.h"
#include <cassert>
#include <iostream>
#include <cinttypes>

void testMoveAndUnmakeMove()
{
    Board board = Board();

    Board boardBackup = board;

    Move move(9ULL, 59ULL, PieceType::blackPawn, PieceType::whiteQueen, PieceType::blackBishop, MoveType::normal);

    board.makeMove(move);
    board.unmakeMove(move);

    assert(board == boardBackup);
}

void testZobristKeyGeneration()
{
    Board board = Board();

    Move move(9ULL, 59ULL, PieceType::blackPawn, PieceType::whiteQueen, PieceType::blackBishop, MoveType::normal);
  
    board.makeMove(move);

    assert(board.getZobristKey() == board.getNewlyGeneratedZobristKey());
}

void thriceRepetitionTest()
{
    Board board = Board();

    Move move(57, 42, PieceType::whiteKnight, PieceType::empty, PieceType::empty, MoveType::normal);

    Move move2(42, 57, PieceType::whiteKnight, PieceType::empty, PieceType::empty, MoveType::normal);

    Move move3(1, 18, PieceType::blackKnight, PieceType::empty, PieceType::empty, MoveType::normal);

    Move move4(18, 1, PieceType::blackKnight, PieceType::empty, PieceType::empty, MoveType::normal);

    assert(board.getGamestate() == GameState::playing);

    for (int i = 0; i < 3; i++)
    {
        board.makeMove(move);

        board.makeMove(move3);

        board.makeMove(move2);

        board.makeMove(move4);
    }

    assert(board.getGamestate() == GameState::draw);

}

#endif
