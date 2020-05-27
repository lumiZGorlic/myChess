#ifndef UCI_H
#define UCI_H

#include "board.h"

#include <sstream>

void go(chessBoard& board, std::istringstream& is);
void position(chessBoard& board, std::istringstream& is);
void uciLoop();


#endif
