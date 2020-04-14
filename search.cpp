#include "defs.h"
#include <iostream>
#include <vector>
#include <chrono>
#include <assert.h>


move searchEngine::doSearch(chessBoard& board, int depth)
{
    assert(depth > 0);

    startTime = std::chrono::system_clock::now();
    timeLimit = 30.0;
    nodes = 0;

    move ret(0, 0, 0);
    chessBoard boardCpy = board;

    for (int i = 1; i <= depth; i++)
    {
        move tmpMv(0, 0, 0);

        try
        {
            tmpMv = searchMain(board, i);
        }
        catch (...)
        {
            board = boardCpy;
            break;
        }

        ret = tmpMv;
    }

    return ret;
};

move searchEngine::searchMain(chessBoard& board, int depth)
{
    nodes++;

    move m(0, 0, 0);
    chessBoard boardCpy;
    int score = -200000, tmp;
 
    move moves[MAX_NUM_OF_MVS];
    board.numOfMvs = 0;
    board.genMoves(moves);

    for (int i = 0; i < board.numOfMvs; i++)
    {
        boardCpy = board;

        if (board.makeMove(moves[i]))
        { 
            tmp = -search(board, depth - 1, -200000, -score);
            if (tmp > score)
            {
                score = tmp;
                m = moves[i];
            }
        }

        board = boardCpy;
    }

    return m;
}

int searchEngine::search(chessBoard& board, int depth, int alpha, int beta)
{
    if (depth == 0)
    {
        return quiesce(board, alpha, beta);
    }

    nodes++;

    if ((nodes & 1023) == 0)
    {
        // time management
        auto endTime = std::chrono::system_clock::now();
        std::chrono::duration<double> elapsedSeconds = endTime - startTime;
        if (elapsedSeconds.count() >= timeLimit)
        {
            std::cout << "elapsed seconds " << elapsedSeconds.count()
                      << std::endl;

            throw 0;
        }
    }

    if (board.currPosRepeats() >= 2)
        return 0;

    bool playedMoves = false;

    move moves[MAX_NUM_OF_MVS];
    board.numOfMvs = 0;
    board.genMoves(moves);

    chessBoard boardCpy;
    int tmp = 0;
 
    for (int i = 0; i < board.numOfMvs; i++)
    {
        boardCpy = board;

        if (board.makeMove(moves[i]))
        {
            playedMoves = true;    

            tmp = -search(board, depth - 1, -beta, -alpha);
            if (tmp >= beta)
                return beta;
            if (tmp > alpha)
                alpha = tmp;
        }

        board = boardCpy;
    }

    if (!playedMoves)
    {
        if (board.isInCheck(board.side))
            return -200000 + depth;
        else
            return 0;
    }

    // if depth 1 in mainSearch will be incorrect so need to handle it in quiesce
    if (board.fifty >= 100)
        return 0;

    return alpha;
}

// TODO seems like there's an explosion here so have to check that
int searchEngine::quiesce(chessBoard& board, int alpha, int beta ) {

    nodes++;

    if ((nodes & 1023) == 0)
    {
        // time management
        auto endTime = std::chrono::system_clock::now();
        std::chrono::duration<double> elapsedSeconds = endTime - startTime;
        if (elapsedSeconds.count() >= timeLimit)
        {
            std::cout << "elapsed seconds " << elapsedSeconds.count()
                      << std::endl;

            throw 0;
        }
    }

    // TODO have to modify it to handle mate etc
    // TODO might be worth adding some margin
    int standPat = eval(board);
    if( standPat >= beta )
        return beta;
    if( alpha < standPat )
        alpha = standPat;

    move moves[MAX_NUM_OF_MVS];
    board.numOfMvs = 0;
    board.genMoves(moves);

    chessBoard boardCpy;
    int tmp = 0;

    // TODO add checks, promotions etc
    // have to add 50 moves rule then ?
    for (int i = 0; i < board.numOfMvs; i++)
    {
        if (board.charBoard[(int)(moves[i].to)] != ' ' || moves[i].flag == EP)
        {
            boardCpy = board;

            if (board.makeMove(moves[i]))
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

