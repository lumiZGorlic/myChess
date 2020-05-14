
#ifndef EVAL_H
#define EVAL_H

#include "defs.h"
#include "board.h"

#define PAWN_VALUE   100
#define KNIGHT_VALUE 320
#define BISHOP_VALUE 330
#define ROOK_VALUE   500
#define QUEEN_VALUE  900


int eval(const chessBoard& board);

#endif
