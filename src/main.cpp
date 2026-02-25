#include <windows.h>
#include <bit>
#include "tests.h"
#include "board.h"

int main()
{
    SetConsoleOutputCP(CP_UTF8);

    testMoveAndUnmakeMove();
    testZobristKeyGeneration();
    thriceRepetitionTest();

    Board board = Board();

    board.draw();

    std::cout << std::countr_zero(0ULL);

    return 0;
}