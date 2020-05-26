#ifndef TESTUTILS_H
#define TESTUTILS_H

#include "defs.h"
#include "board.h"

#include <vector>


void uciLoop(int argc, char* argv[]);


struct debugItem {
    std::string pos;
    std::vector <U64> perftResults;
    debugItem(const std::string& p, const std::vector<U64>& perftRes)
             : pos(p) , perftResults(perftRes)
    { }
};

U64 perft(chessBoard& board, int depth);
U64 perft2();
void debugMoveGen ();


bool pickRandomMove(chessBoard& board, move& m, int prefMv = 0);
void printMove(const move& m);
int pickSearchDepth();
move getInput();
int promotionPickNewPiece();
bool gameEnded(chessBoard board);
void testMoveGenSpeed();
void playDummyGame();
void solveChessPuzzle(std::string pos);
void engineVsEngine();
void playChess ();
void testSearch();
void bratkoKopec();


#endif
