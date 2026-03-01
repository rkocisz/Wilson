#ifndef TESTS_H
#define TESTS_H

#include "board.h"
#include "enums.h"
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

long long perftRecursion(int depth, Board& board)
{
    if (depth == 0)
    {
        return 1;
    }

    std::vector<Move> legalMoves = board.generateLegalMoves();
    long long movesCount = 0;
    
    for (Move& move : legalMoves)
    {
        board.makeMove(move);
        movesCount += perftRecursion(depth - 1, board);
        board.unmakeMove(move);
    }

    return movesCount;
}

void perft()
{
    Board board = Board();

    std::cout << perftRecursion(3, board);

    assert(perftRecursion(1, board) == 20);
    assert(perftRecursion(2, board) == 400);
    assert(perftRecursion(3, board) == 8902);
    assert(perftRecursion(4, board) == 197281);
    assert(perftRecursion(5, board) == 4865609);
    assert(perftRecursion(6, board) == 119060324);
}

#endif
