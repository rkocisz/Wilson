#include <windows.h>
#include <bitset>
#include <vector>

#include "tests.h"
#include "board.h"
#include "util.h"

int main()
{
    SetConsoleOutputCP(CP_UTF8);

    Util::initUtil();

    testMoveAndUnmakeMove();
    testZobristKeyGeneration();
    thriceRepetitionTest();
    perft();

    return 0;
}