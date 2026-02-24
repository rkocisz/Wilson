#include <windows.h>
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

    return 0;
}