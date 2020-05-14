#include "defs.h"
#include "search.h"
#include "tt.h"
#include "board.h"
#include "eval.h"

#include <iostream>
#include <chrono>
#include <assert.h>


void sortMoves(move* moves, int numOfMvs, int i)
{
    int mxScore = moves[i].score;
    int mxScoreIdx = i;

    for (int j = i+1; j < numOfMvs; j++)
        if (moves[j].score > mxScore)
        {
            mxScore = moves[j].score;
            mxScoreIdx = j;
        }

    move tmp = moves[i];
    moves[i] = moves[mxScoreIdx];
    moves[mxScoreIdx] = tmp;
}


move searchEngine::doSearch(chessBoard& board, int depth)
{
    assert(depth > 0);

    startTime = std::chrono::system_clock::now();
    timeLimit = 300.0;
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
    int bestScore = -20000000, score;

    move moves[MAX_NUM_OF_MVS];
    board.numOfMvs = 0;
    board.genMoves(moves);

    chessBoard boardCpy = board;

    for (int i = 0; i < board.numOfMvs; i++)
    {
        sortMoves(moves, board.numOfMvs, i);

        if (board.makeMove(moves[i]))
        {
            score = -search(board, depth - 1, -20000000, -bestScore);
            if (score > bestScore)
            {
                bestScore = score;
                m = moves[i];
            }
        }

        board = boardCpy;
    }

    tt.save(board.currPosHash, depth, FULL, bestScore, 20000000);

    return m;
}

int searchEngine::search(chessBoard& board, int depth, int alpha, int beta)
{
    //assert(alpha < beta);
    int origAlpha = alpha;

    int ttScore = tt.probe(board.currPosHash, depth, alpha, beta);
    if (ttScore != NO_TT_ENTRY)
        return ttScore;

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

    chessBoard boardCpy = board;
    int score = 0;

    for (int i = 0; i < board.numOfMvs; i++)
    {
        sortMoves(moves, board.numOfMvs, i);

        if (board.makeMove(moves[i]))
        {
            playedMoves = true;

            score = -search(board, depth - 1, -beta, -alpha);
            if (score >= beta)
            {
                // needs to be boardCpy as board's been modified by this point
                tt.save(boardCpy.currPosHash, depth, BETA, alpha, beta);
                return beta;
            }
            if (score > alpha)
                alpha = score;
        }

        board = boardCpy;
    }

    if (!playedMoves)
    {
        if (board.isInCheck(board.side))
            // trick so that check mates in smaller number of moves get higher
            // score. -19999970 seems safe. Max number used as alpha in
            // search is 20000000. And search unlikely to be deeper than 30.
            return -19999970 - depth;
        else
            return 0;
    }

    // if depth 1 in mainSearch will be incorrect so need to handle in quiesce
    if (board.fifty >= 100)
        return 0;

    if (alpha == origAlpha)
        tt.save(board.currPosHash, depth, ALPHA, alpha, beta);
    else
        tt.save(board.currPosHash, depth, FULL,  alpha, beta);

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

    chessBoard boardCpy = board;
    int score = 0;

    // TODO add checks, promotions etc
    // have to add 50 moves rule then ?
    for (int i = 0; i < board.numOfMvs; i++)
    {
        // quiesce needs to have its own sort procedure and even
        // better a separate move generation
        sortMoves(moves, board.numOfMvs, i);

        if (board.charBoard[(int)(moves[i].to)] != ' ' || moves[i].flag == EP)
        {
            if (board.makeMove(moves[i]))
            {
                 score = -quiesce(board, -beta, -alpha);
                 if (score >= beta)
                     return beta;
                 if (score > alpha)
                     alpha = score;
            }

            board = boardCpy;
        }
    }

    return alpha;
}

