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

    //Util::dobraTrzebaZtestowac();

    /*testMoveAndUnmakeMove();
    testZobristKeyGeneration();
    thriceRepetitionTest();*/
    //perft();

    Board board = Board();

    std::vector<Move> moves = board.generateLegalMoves();

    std::cout << std::bitset<64>(board.getAllPiecesOccupancy()) << "\n\n";

    for (Move move : moves)
    {
        //std::cout << move.moved << " from: " << move.startPos << " to: " << move.endPos << "\n";
    
        std::bitset<64> bitset(Util::squareMask(move.endPos));
        for (int j = 0; j < 64; j++)
        {
            std::cout << bitset[j] << " ";
            if ((j + 1) % 8 == 0)
                std::cout << "\n";
        }
        std::cout << "\n";
    }

    


    return 0;
}