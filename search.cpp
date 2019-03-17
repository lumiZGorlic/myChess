#include "defs.h"
#include <iostream>
#include <vector>
#include <chrono>


move searchEngine::doSearch(chessBoard& board, int depth)
{
    // TODO sanity check for depth 

    startTime = std::chrono::system_clock::now();
    stopSearch = false;
    timeLimit = 30.0;

    move ret(0, 0, 0);
    for (int i = 1; i <= depth; i++)
    {
        move tmpMv(0, 0, 0);
        tmpMv = searchMain(board, i);
        // only copy the move if the search was full as otherwise it won't
        // return the best move
        if (stopSearch == false)
            ret = tmpMv;
    }

    // TODO need to improve as it still can return a silly move
    return ret;
};

move searchEngine::searchMain(chessBoard& board, int depth)
{
    // TODO special case if depth zero - make sure it never happens ?
    // TODO make sure we don't get here if mate or stalemate ?

    std::vector<move> moves; std::vector<move>::iterator it;
    move m(0, 0, 0);
    chessBoard boardCpy;
    int score = -200000, tmp;
 
    board.genMoves(moves);

    for (it = moves.begin(); it != moves.end(); it++)
    {
        boardCpy = board;

        if (board.makeMove(*it))
        { 
            tmp = -search(board, depth - 1, -200000, -score);
            if (tmp > score)
            {
                score = tmp;
                m = *it;
            }
        }

        board = boardCpy;

        if (stopSearch == true)
            return m;
    }

    // TODO just debug
    std::cout << "score is ---> " << score << std::endl;
    return m;
}


int searchEngine::search(chessBoard& board, int depth, int alpha, int beta)
{
    std::vector<move> moves; std::vector<move>::iterator it;
    chessBoard boardCpy;
    int tmp = 0;
 
    if (depth == 0)
    {
        // TODO not sure how big the overhead is. Maybe it shouldn't be called
        // so frequently.

        // time management
        auto endTime = std::chrono::system_clock::now();
        std::chrono::duration<double> elapsedSeconds = endTime - startTime;
        if (elapsedSeconds.count() >= timeLimit)
        {
            std::cout << "elapsed seconds " << elapsedSeconds.count() << std::endl;

            stopSearch = true;
            return 0;
        }

        // TODO debug
        return quiesce(board, alpha, beta);
    }

    // TODO justify why it's here and not below
    if (board.currPosRepeats() >= 2)
        return 0;

    board.genMoves(moves);

    bool playedMoves = false;
    for (it = moves.begin(); it != moves.end(); it++)
    {
        boardCpy = board;

        if (board.makeMove(*it))
        {
            playedMoves = true;    

            tmp = -search(board, depth - 1, -beta, -alpha);
            if (tmp >= beta)
                return beta;
            if (tmp > alpha)
                alpha = tmp;
        }

        board = boardCpy;

        if (stopSearch == true)
            return 0;
    }

    if (!playedMoves)
    {
        if (board.isInCheck(board.side))
            return -200000 + depth;
        else
            return 0;
    }

    // TODO check if this can/should be put closer to the top
    // so that '0' is returned before the loop
    // TODO: also if depth is '1' in mainSearch then probably a bug so should
    // add some code in quiesce (?) to handle it
    if (board.fifty >= 100)
        return 0;

    return alpha;
}

// TODO seems like there's an explosion here so have to check that
int searchEngine::quiesce(chessBoard& board, int alpha, int beta ) {
    std::vector<move> moves; std::vector<move>::iterator it;
    chessBoard boardCpy;
    int tmp = 0;

    // TODO have to modify it to handle mate etc
    // TODO might be worth adding some margin
    int standPat = eval(board);
    if( standPat >= beta )
        return beta;
    if( alpha < standPat )
        alpha = standPat;

    board.genMoves(moves);

    // TODO add checks, promotions etc
    // have to add 50 moves rule then ?
    for (it = moves.begin(); it != moves.end(); it++)
    {
        if (board.charBoard[(int)(it->to)] != ' ' || it->flag == EP)
        {
            boardCpy = board;

            if (board.makeMove(*it))
            {
                 tmp = -quiesce(board, -beta, -alpha);
                 if (tmp >= beta)
                     return beta;
                 if (tmp > alpha)
                     alpha = tmp;
            }

            board = boardCpy;
        }
    }

    return alpha;
}

