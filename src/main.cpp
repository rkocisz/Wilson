#include <windows.h>
#include <bit>
#include <bitset>
#include "tests.h"
#include "board.h"
#include "structs.h"
#include "util.h"

int main()
{
    SetConsoleOutputCP(CP_UTF8);

    testMoveAndUnmakeMove();
    testZobristKeyGeneration();
    thriceRepetitionTest();
    perft();


    return 0;
}