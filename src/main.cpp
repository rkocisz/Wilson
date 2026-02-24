#include <windows.h>
#include "tests.h"
#include "board.h"

int main()
{
    SetConsoleOutputCP(CP_UTF8);

    testMoveAndUnmakeMove();
    testZobristKeyGeneration();

    Board board = Board();

    board.loadStartPos();
    board.draw();

    return 0;
}