#ifndef TESTS_H
#define TESTS_H

#include "board.h"
#include "enums.h"
#include "moveGen.h"
#include "common.h"
#include "evaluation.h"
#include "util.h";
#include "search.h"

#include <cassert>
#include <iostream>
#include <cinttypes>
#include <chrono>

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

    std::vector<Move> legalMoves = MoveGen::generateLegalMoves(&board);
    long long movesCount = 0;
    

    if (depth == 1)
        return legalMoves.size();

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

    auto start = std::chrono::high_resolution_clock::now();


    if (perftRecursion(1, board) == 20)
    {
        std::cout << "Test passed!\n";
    }
    else
    {
        std::cout << "Test NOT passed!\n";
    }

    if (perftRecursion(2, board) == 400)
    {
        std::cout << "Test passed!\n";
    }
    else
    {
        std::cout << "Test NOT passed!\n";
    }

    if (perftRecursion(3, board) == 8902)
    {
        std::cout << "Test passed!\n";
    }
    else
    {
        std::cout << "Test NOT passed!\n";
    }

    if (perftRecursion(4, board) == 197281)
    {
        std::cout << "Test passed!\n";
    }
    else
    {
        std::cout << "Test NOT passed!\n";
    }

    if (perftRecursion(5, board) == 4865609)
    {
        std::cout << "Test passed!\n";
    }
    else
    {
        std::cout << "Test NOT passed!\n";
    }

    if (perftRecursion(6, board) == 119060324)
    {
        std::cout << "Test passed!\n";
    }
    else
    {
        std::cout << "Test NOT passed!\n";
    }

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;    

    std::cout << "perft took: " << elapsed.count() << " seconds\n";
    std::cout << "nodes per second: " << static_cast<uint64_t>(124132536 / elapsed.count()) << "\n";

}

void testEval()
{
    Board board;

    std::vector<Move> moves = MoveGen::generateLegalMoves(&board);

    if (Eval::evaluate(board) == 0)
    {
        std::cout << "Test passed!\n";
    }
    else
    {
        std::cout << "Test NOT passed!\n";
    }

    board = Board();
    board.makeMove(moves[0]);
    int eval1 = Eval::evaluate(board);
    board.unmakeMove(moves[0]);

    Move move = moves[0];
    move.moved = PieceType::blackPawn;
    move.startPos ^= 56;
    move.endPos ^= 56;

    board.makeMove(move);
    int eval2 = Eval::evaluate(board);
    board.unmakeMove(move);

    if (eval1 + eval2 == 0)
    {
        std::cout << "Test passed!\n";
    }
    else
    {
        std::cout << "Test NOT passed!\n";
    }
}

void searchPerft()
{
    Board board;

    auto start = std::chrono::high_resolution_clock::now();
    
    board.makeMove(findBestMove(board, 6));
    board.makeMove(findBestMove(board, 6));
    board.makeMove(findBestMove(board, 6));
    board.makeMove(findBestMove(board, 6));
    board.makeMove(findBestMove(board, 6));

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;

    std::cout << "searchPerft took: " << elapsed.count() << " seconds\n";
}


void testIncrementalEval()
{
    Board board;

    if (Eval::calculateUpdatedEval(board) == Eval::evaluate(board))
    {
        std::cout << "Test passed!\n";
    }
    else
    {
        std::cout << "Test NOT passed!\n";
    }

    Move move = MoveGen::generateLegalMoves(&board)[6];
    board.makeMove(move);

    if (Eval::calculateUpdatedEval(board) == Eval::evaluate(board))
    {
        std::cout << "Test passed!\n";
    }
    else
    {
        std::cout << "Test NOT passed!\n";
    }

    Move move1 = MoveGen::generateLegalMoves(&board)[5];
    board.makeMove(move1);
    
    if (Eval::calculateUpdatedEval(board) == Eval::evaluate(board))
    {
        std::cout << "Test passed!\n";
    }
    else
    {
        std::cout << "Test NOT passed!\n";
    }
    
    board.unmakeMove(move1);

    if (Eval::calculateUpdatedEval(board) == Eval::evaluate(board))
    {
        std::cout << "Test passed!\n";
    }
    else
    {
        std::cout << "Test NOT passed!\n";
    }
}

#endif
