#include "defs.h"

#define PAWN_VALUE   100
#define KNIGHT_VALUE 320
#define BISHOP_VALUE 330
#define ROOK_VALUE   500
#define QUEEN_VALUE  900

int mapping[] = { 56, 57, 58, 59, 60, 61, 62, 63,
                  48, 49, 50, 51, 52, 53, 54, 55,
                  40, 41, 42, 43, 44, 45, 46, 47,
                  32, 33, 34, 35, 36, 37, 38, 39,
                  24, 25, 26, 27, 28, 29, 30, 31,
                  16, 17, 18, 19, 20, 21, 22, 23,
                   8,  9, 10, 11, 12, 13, 14, 15,
                   0,  1,  2,  3,  4,  5,  6,  7 };

// pawn
int pawnPtsSqr [] = {
         0,  0,  0,  0,  0,  0,  0,  0,
        50, 50, 50, 50, 50, 50, 50, 50,
        10, 10, 20, 30, 30, 20, 10, 10,
         5,  5, 10, 25, 25, 10,  5,  5,
         0,  0,  0, 20, 20,  0,  0,  0,
         5, -5,-10,  0,  0,-10, -5,  5,
         5, 10, 10,-20,-20, 10, 10,  5,
         0,  0,  0,  0,  0,  0,  0,  0 };

// knight
int knightPtsSqr [] = {
        -50,-40,-30,-30,-30,-30,-40,-50,
        -40,-20,  0,  0,  0,  0,-20,-40,
        -30,  0, 10, 15, 15, 10,  0,-30,
        -30,  5, 15, 20, 20, 15,  5,-30,
        -30,  0, 15, 20, 20, 15,  0,-30,
        -30,  5, 10, 15, 15, 10,  5,-30,
        -40,-20,  0,  5,  5,  0,-20,-40,
        -50,-40,-30,-30,-30,-30,-40,-50 };

// bishop
int bishopPtsSqr [] = {
        -20,-10,-10,-10,-10,-10,-10,-20,
        -10,  0,  0,  0,  0,  0,  0,-10,
        -10,  0,  5, 10, 10,  5,  0,-10,
        -10,  5,  5, 10, 10,  5,  5,-10,
        -10,  0, 10, 10, 10, 10,  0,-10,
        -10, 10, 10, 10, 10, 10, 10,-10,
        -10,  5,  0,  0,  0,  0,  5,-10,
        -20,-10,-10,-10,-10,-10,-10,-20 };

//rook
int rookPtsSqr [] = {
          0,  0,  0,  0,  0,  0,  0,  0,
          5, 10, 10, 10, 10, 10, 10,  5,
         -5,  0,  0,  0,  0,  0,  0, -5,
         -5,  0,  0,  0,  0,  0,  0, -5,
         -5,  0,  0,  0,  0,  0,  0, -5,
         -5,  0,  0,  0,  0,  0,  0, -5,
         -5,  0,  0,  0,  0,  0,  0, -5,
          0,  0,  0,  5,  5,  0,  0,  0 };

//queen
int queenPtsSqr [] = {
        -20,-10,-10, -5, -5,-10,-10,-20,
        -10,  0,  0,  0,  0,  0,  0,-10,
        -10,  0,  5,  5,  5,  5,  0,-10,
         -5,  0,  5,  5,  5,  5,  0, -5,
          0,  0,  5,  5,  5,  5,  0, -5,
        -10,  5,  5,  5,  5,  5,  0,-10,
        -10,  0,  5,  0,  0,  0,  0,-10,
        -20,-10,-10, -5, -5,-10,-10,-20 };

//king middle game
int kingPtsSqr [] = {
        -30,-40,-40,-50,-50,-40,-40,-30,
        -30,-40,-40,-50,-50,-40,-40,-30,
        -30,-40,-40,-50,-50,-40,-40,-30,
        -30,-40,-40,-50,-50,-40,-40,-30,
        -20,-30,-30,-40,-40,-30,-30,-20,
        -10,-20,-20,-20,-20,-20,-20,-10,
         20, 20,  0,  0,  0,  0, 20, 20,
         20, 30, 10,  0,  0, 10, 30, 20 };

// TODO: should have also king end game

int eval(const chessBoard& board)
{
    int score = 0;

    for (int i=A1; i <= H8; i++)
    {
        if (board.charBoard[i] == ' ')
        {
            continue;
        }
        else if (board.charBoard[i] == 'R')
        {
            score += ROOK_VALUE;
            score += rookPtsSqr[mapping[i]];
        }
        else if (board.charBoard[i] == 'N')
        {
            score += KNIGHT_VALUE;
            score += knightPtsSqr[mapping[i]];
        }
        else if (board.charBoard[i] == 'B')
        {
            score += BISHOP_VALUE;
            score += bishopPtsSqr[mapping[i]];
        }
        else if (board.charBoard[i] == 'Q')
        {
            score += QUEEN_VALUE;
            score += queenPtsSqr[mapping[i]];
        }
        else if (board.charBoard[i] == 'P')
        {
            score += PAWN_VALUE;
            score += pawnPtsSqr[mapping[i]];
        }
        else if (board.charBoard[i] == 'r')
        {
            score -= ROOK_VALUE;
            score -= rookPtsSqr[i];
        }
        else if (board.charBoard[i] == 'n')
        {
            score -= KNIGHT_VALUE;
            score -= knightPtsSqr[i];
        }
        else if (board.charBoard[i] == 'b')
        {
            score -= BISHOP_VALUE;
            score -= bishopPtsSqr[i];
        }
        else if (board.charBoard[i] == 'q')
        {
            score -= QUEEN_VALUE;
            score -= queenPtsSqr[i];
        }
        else if (board.charBoard[i] == 'p')
        {
            score -= PAWN_VALUE;
            score -= pawnPtsSqr[i];
        }
    }


    if (board.side == false)
        score = -score;

    return score;;
}

