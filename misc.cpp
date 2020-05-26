#include "defs.h"
#include "misc.h"
#include "board.h"

#include <iostream>


// make it inline ??
int rank(int sqr)
{
    return sqr / 8;
}

int file(int sqr)
{
    return sqr % 8;
}

int square(int rank, int file)
{
    return 8 * rank + file;
}

bool rankFileOK(int s)
{
    return (s >= 0 && s <= 7) ? true : false;
}

bool squareOK(int s)
{
    return (s >= A1 && s <= H8) ? true : false;
}

// TODO: also should have sth like resetBit which could
// help simplify the code
void setBit(U64& b, int sqr)
{
    // TODO: could use pre-prepared singleBit[64]
    U64 u = 1;
    b |= (u << sqr);
}


void printBoard(const U64& b)
{
    // code for printing the chess board
    for (int i = 7; i >= 0; i--)
    {
        for (int j = 0; j <= 7; j++)
             std::cout << ((b >> (8*i+j)) & 1);

        std::cout << std::endl;
    }
}

void printBoard2(const U64& b)
{
    // code for printing the chess board
    for (int i = 7; i >= 0; i--)
    {
        std::cout << "------------------------------" << std::endl;

        for (int j = 0; j <= 7; j++)
             std::cout << ((b >> (8*i+j)) & 1) << " | ";

        std::cout << std::endl;
    }

}


bool savePosInFEN(chessBoard& board, std::string& pos)
{
    int sqrMapFENotation[64] = {
                       56, 57, 58, 59, 60, 61, 62, 63,
                       48, 49, 50, 51, 52, 53, 54, 55,
                       40, 41, 42, 43, 44, 45, 46, 47,
                       32, 33, 34, 35, 36, 37, 38, 39,
                       24, 25, 26, 27, 28, 29, 30, 31,
                       16, 17, 18, 19, 20, 21, 22, 23,
                       8,  9,  10, 11, 12, 13, 14, 15,
                       0,  1,  2,  3,  4,  5,  6,  7  };

    int emptySqrs = 0;
    for (int i = A1; i <= H8; i++)
    {
        if (board.charBoard[sqrMapFENotation[i]] == ' ')
        {
            emptySqrs++;
        }
        else
        {
            if (emptySqrs > 0)
                pos += std::to_string(emptySqrs);
            pos += board.charBoard[sqrMapFENotation[i]];

            emptySqrs = 0;
        }

        if(i % 8 == 7)
        {
            if (emptySqrs > 0)
                pos += std::to_string(emptySqrs);
            if (i != 63)
                pos += "/";

            emptySqrs = 0;
        }
    }

    if (board.side == true)
        pos  += " w ";
    else if (board.side == false)
        pos  += " b ";

    if (board.castlRights[0] == 1)
        pos += "K";
    if (board.castlRights[1] == 1)
        pos += "Q";
    if (board.castlRights[2] == 1)
        pos += "k";
    if (board.castlRights[3] == 1)
        pos += "q";

    if (pos.back() != ' ')
        pos += " ";

    // TODO finish
    if (board.ep == -1)
        pos += "- ";
    else
    {

    }

    // TODO should return false if unsuccessful
    return true;
}

bool readInPosFromFEN(chessBoard& board, std::string pos)
{
    // TODO: could add string verification (regular expression etc)
    int i=0, square=0;

    int sqrMapFENotation[64] = {
                       56, 57, 58, 59, 60, 61, 62, 63,
                       48, 49, 50, 51, 52, 53, 54, 55,
                       40, 41, 42, 43, 44, 45, 46, 47,
                       32, 33, 34, 35, 36, 37, 38, 39,
                       24, 25, 26, 27, 28, 29, 30, 31,
                       16, 17, 18, 19, 20, 21, 22, 23,
                       8,  9,  10, 11, 12, 13, 14, 15,
                       0,  1,  2,  3,  4,  5,  6,  7  };

    do {
        if (pos[i] == '/')
        {
        }
        else if (pos[i] == 'K')
        {
            setBit(board.wKing_Occ, sqrMapFENotation[square]);
            square++;
        }
        else if (pos[i] == 'Q')
        {
            setBit(board.wQueen_Occ, sqrMapFENotation[square]);
            square++;
        }
        else if (pos[i] == 'R')
        {
            setBit(board.wRook_Occ, sqrMapFENotation[square]);
            square++;
        }
        else if (pos[i] == 'B')
        {
            setBit(board.wBishop_Occ, sqrMapFENotation[square]);
            square++;
        }
        else if (pos[i] == 'N')
        {
            setBit(board.wKnight_Occ, sqrMapFENotation[square]);
            square++;
        }
        else if (pos[i] == 'P')
        {
            setBit(board.wPawn_Occ, sqrMapFENotation[square]);
            square++;
        }
        else if (pos[i] == 'k')
        {
            setBit(board.bKing_Occ, sqrMapFENotation[square]);
            square++;
        }
        else if (pos[i] == 'q')
        {
            setBit(board.bQueen_Occ, sqrMapFENotation[square]);
            square++;
        }
        else if (pos[i] == 'r')
        {
            setBit(board.bRook_Occ, sqrMapFENotation[square]);
            square++;
        }
        else if (pos[i] == 'b')
        {
            setBit(board.bBishop_Occ, sqrMapFENotation[square]);
            square++;
        }
        else if (pos[i] == 'n')
        {
            setBit(board.bKnight_Occ, sqrMapFENotation[square]);
            square++;
        }
        else if (pos[i] == 'p')
        {
            setBit(board.bPawn_Occ, sqrMapFENotation[square]);
            square++;
        }
        else
        {
            int ic = pos[i] - '0';
            square += ic;
        }
        i++;

    } while (pos[i] != ' ');

    i++;
    if (pos[i] == 'w')
        board.side = true;
    else
        board.side = false;

    i += 2;

    // all bits set to zero
    board.castlRights.reset();

    if (pos[i] == '-')
        i++;
    else
    {
        while (pos[i] != ' ')
        {
            switch (pos[i])
            {
                case 'K':
                    board.castlRights[0] = true;
                    break;
                case 'Q':
                    board.castlRights[1] = true;
                    break;
                case 'k':
                    board.castlRights[2] = true;
                    break;
                case 'q':
                    board.castlRights[3] = true;
                    break;
            }
            i++;
        }
    }

    i++;

    if (pos[i] == '-')
        i++;
    else
    {
        board.ep = (pos[i] - 'a') + 8*(pos[i+1] - '1');
        i += 2;
    }

    i++;

    // TODO should return false if unsuccessful
    return true;
}
