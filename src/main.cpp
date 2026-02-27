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

    Board board = Board();

    board.draw();

	std::vector<Move> moves = board.generatePseudoLegalMoves();

	for (Move move : moves)
    {
        std::cout << "Move from " << move.startPos << " to " << move.endPos << std::endl;
    }

    return 0;
}