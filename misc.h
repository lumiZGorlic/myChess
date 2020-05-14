
#ifndef MISC_H
#define MISC_H

#include "defs.h"
#include "board.h"


int rank(int sqr);
int file(int sqr);
int square(int rank, int file);
bool rankFileOK(int s);
bool squareOK(int s);
void setBit(U64& b, int sqr);

void printBoard(const U64& b);
void printBoard2(const U64& b);

bool savePosInFEN(chessBoard& board, std::string& pos);
bool readInPosFromFEN(chessBoard& board, std::string pos);


#endif
