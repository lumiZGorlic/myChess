
#ifndef SEARCH_H
#define SEARCH_H

#include "defs.h"
#include "board.h"

#include <chrono>

struct searchEngine {
    std::chrono::time_point<std::chrono::system_clock> startTime;
    double timeLimit;
    unsigned long long int nodes;

    move doSearch(chessBoard& board, int depth);
    move searchMain(chessBoard& board, int depth);
    int search(chessBoard& board, int depth, int alpha, int beta);
    int quiesce(chessBoard& board, int alpha, int beta );
};


#endif
