#include <windows.h>
#include <bitset>
#include <vector>

#include "tests.h"
#include "board.h"
#include "util.h"
#include "moveGenUtil.h"
#include "evaluation.h"

int main()
{
    SetConsoleOutputCP(CP_UTF8);

    std::cout << "♔ ♕ ♖ ♗ ♘ ♙" << std::endl;

    

    MoveGenUtil::init();
    Eval::init();

    //testMoveAndUnmakeMove();
    //testZobristKeyGeneration();
    //thriceRepetitionTest();
    //perft();

    Board board;
    board.draw();
    std::cout << "\n\neval: " << Eval::evaluate(board);

    return 0;
}