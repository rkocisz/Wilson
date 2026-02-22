#include <iostream>
#include <windows.h>
#include "board.h"
#include <cassert>

int main()
{
    SetConsoleOutputCP(CP_UTF8);

    Board board = Board();

    Board boardBackup = board;

    board.draw();

    Move move;
    move.captured = PieceType::whiteQueen;
    move.startPos = 9;
    move.endPos = 59;
    move.promotion = PieceType::blackBishop;
    move.moveType = MoveType::normal;
    move.moved = PieceType::blackPawn;

    board.makeMove(move);
    board.draw();

    board.unmakeMove(move);
    board.draw();

    assert(board == boardBackup);

    return 0;
}