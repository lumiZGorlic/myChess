#include "defs.h"
#include <random>
#include <iostream>
#include <fstream>
#include <vector>
#include <set>
#include <bitset>

// TODO: should int be used for square or some other type
// TODO: int should be unsigned or not ? why ?
// TODO: more enums, for example for 'side' etc
// TODO: pass reference to const where appropriate
// TODO: also some functions should be const e.g. genMoves etc
// TODO: think about what should be inline and what shouldn't


std::string sqrMapIntToStr[] = {
   "A1", "B1", "C1", "D1", "E1", "F1", "G1", "H1",
   "A2", "B2", "C2", "D2", "E2", "F2", "G2", "H2",
   "A3", "B3", "C3", "D3", "E3", "F3", "G3", "H3",
   "A4", "B4", "C4", "D4", "E4", "F4", "G4", "H4",
   "A5", "B5", "C5", "D5", "E5", "F5", "G5", "H5",
   "A6", "B6", "C6", "D6", "E6", "F6", "G6", "H6",
   "A7", "B7", "C7", "D7", "E7", "F7", "G7", "H7",
   "A8", "B8", "C8", "D8", "E8", "F8", "G8", "H8"
};


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


chessBoard::chessBoard() :
    occ(0),
    white_Occ(0), black_Occ(0),
    wKing_Occ(0), wQueen_Occ(0), wRook_Occ(0), wBishop_Occ(0), wKnight_Occ(0), wPawn_Occ(0),
    bKing_Occ(0), bQueen_Occ(0), bRook_Occ(0), bBishop_Occ(0), bKnight_Occ(0), bPawn_Occ(0),
    side(true),
    castlRights(std::string("1111")),
    ep(-1),
    fifty(0),
    currPosHash(0)
{
    for (int i = A1; i <= H8; i++)
        charBoard[i] = ' ';

    for (int i = A1; i <= H8; i++)
        for (int j = 0; j <= 5; j++)
            for (int k = 0; k <=1; k++)
                squaresHash[i][j][k] = genHash();

    for (int i = A1; i <= H8; i++)
        epHash[i] = genHash();

    sideHash = genHash();
}

// TODO investigate why this needs to be global etc
std::mt19937 mt(01234567);

unsigned long long int chessBoard::genHash()
{
    // taken from the article on zobrist hashing on geeksforgeeks
    std::uniform_int_distribution<unsigned long long int> dist(0, UINT64_MAX); 
    return dist(mt); 
}

void chessBoard::calcHash()
{
    currPosHash = 0;

    int sideInt = (side == true) ? 0 : 1;
    for  (int i = A1; i <= H8; i++)
    {
        if (charBoard[i] == 'R' || charBoard[i] == 'r')
            currPosHash ^= squaresHash[i][0][sideInt];
        else if (charBoard[i] == 'N' || charBoard[i] == 'n')
            currPosHash ^= squaresHash[i][1][sideInt];
        else if (charBoard[i] == 'B' || charBoard[i] == 'b')
            currPosHash ^= squaresHash[i][2][sideInt];
        else if (charBoard[i] == 'Q' || charBoard[i] == 'q')
            currPosHash ^= squaresHash[i][3][sideInt];
        else if (charBoard[i] == 'K' || charBoard[i] == 'k')
            currPosHash ^= squaresHash[i][4][sideInt];
        else if (charBoard[i] == 'P' || charBoard[i] == 'p')
            currPosHash ^= squaresHash[i][5][sideInt];
    }
    currPosHash ^= sideHash;
    if (ep != -1)
        currPosHash ^= epHash[ep];

    historyHash.push_back(currPosHash);
}

int chessBoard::currPosRepeats()
{
    // TODO Should be tested and incorporated into search and playing routines

    // TODO print just for debugging.
    //for (int i = 0; i < (int)historyHash.size(); i++)
    //    std::cout << "pos nr " << i << " and the hash is " << historyHash[i] << std::endl;

    int ret = 0;
    for (int i = 0; i < (int)historyHash.size(); i++)
        if (historyHash[i] == currPosHash)
            ret++;
    //std::cout << "curretn pos repeated " << ret << std::endl;

    return ret;
}

void chessBoard::populateOccup () {
    white_Occ = wKing_Occ | wQueen_Occ | wRook_Occ | wBishop_Occ | wKnight_Occ | wPawn_Occ;
    black_Occ = bKing_Occ | bQueen_Occ | bRook_Occ | bBishop_Occ | bKnight_Occ | bPawn_Occ;
    occ = white_Occ | black_Occ;
}

void chessBoard::populateCharBoardHelper(U64 b, char c) {

    for (int i = A1; i <= H8; i++)
    {
        if((b >> i) & 1)
            charBoard[i] = c;
    }
}

// TODO: should be inline ? also could be moved to readIn func so that
// everyting is in one place
void chessBoard::populateCharBoard() {
    populateCharBoardHelper(wRook_Occ, 'R');
    populateCharBoardHelper(wKnight_Occ, 'N');
    populateCharBoardHelper(wBishop_Occ, 'B');
    populateCharBoardHelper(wQueen_Occ, 'Q');
    populateCharBoardHelper(wKing_Occ, 'K');
    populateCharBoardHelper(wPawn_Occ, 'P');

    populateCharBoardHelper(bRook_Occ, 'r');
    populateCharBoardHelper(bKnight_Occ, 'n');
    populateCharBoardHelper(bBishop_Occ, 'b');
    populateCharBoardHelper(bQueen_Occ, 'q');
    populateCharBoardHelper(bKing_Occ, 'k');
    populateCharBoardHelper(bPawn_Occ, 'p');

}

void chessBoard::printCharBoard ()
{
    int viewMap[64] = {
               A8, B8, C8, D8, E8, F8, G8, H8,
               A7, B7, C7, D7, E7, F7, G7, H7,
               A6, B6, C6, D6, E6, F6, G6, H6,
               A5, B5, C5, D5, E5, F5, G5, H5,
               A4, B4, C4, D4, E4, F4, G4, H4,
               A3, B3, C3, D3, E3, F3, G3, H3,
               A2, B2, C2, D2, E2, F2, G2, H2,
               A1, B1, C1, D1, E1, F1, G1, H1  };

    for (int i = 0; i <= 63; i++)
    {
        char piece = charBoard[viewMap[i]];
        if (piece == ' ')
            std::cout << ".";
        else
            std::cout << piece;

        if (i % 8 == 7)
            std::cout << std::endl;
    }

    std::cout << std::endl << std::endl;
}


int chessBoard::squareWithKing(bool sideToFind)
{
    int ret = -1;

    for (int i=A1; i<=H8; i++)
    {
        if (sideToFind == true && charBoard[i] == 'K')
        {
            ret = i;
            break;
        }
        else if (sideToFind == false && charBoard[i] == 'k')
        {
            ret = i;
            break;
        }
    }

    return ret;
}

bool chessBoard::isInCheck(bool sideToFind){

    int sqrKing = squareWithKing(sideToFind);
    std::vector<move> moves; std::vector<move>::iterator it;
    genMoves(moves);
    for (it = moves.begin(); it != moves.end(); it++)
    {
        if ((int)it->to == sqrKing)
        {
            return true;
        }
    }

    return false;
}

void chessBoard::calcAttackSqrRook(std::vector<move>& mvs, int sqr)
{
    int j = sqr;
    j += 8;
    while ( squareOK(j) )
    {
        // TODO: could have a separate function to avoid code duplication
        // or better use tab[-8, 8, -1, 1] and only one loop 'while'
        if (side == true)
        {
            // TODO: compare cost of this to just checking charBoard
            if ((white_Occ >> j) & 1)
            {
                break;
            }
            else if ((black_Occ >> j) & 1)
            {
                mvs.push_back(move(sqr, j, 0));
                break;
            }
            else
            {
                mvs.push_back(move(sqr, j, 0));
            }
        }
        else
        {
            if ((black_Occ >> j) & 1)
            {
                break;
            }
            else if ((white_Occ >> j) & 1)
            {
                mvs.push_back(move(sqr, j, 0));
                break;
            }
            else
            {
                mvs.push_back(move(sqr, j, 0));
            }
        }

        j += 8;
    }

    j = sqr;
    j -= 8;
    while ( squareOK(j) )
    {
        if (side == true)
        {
            if ((white_Occ >> j) & 1)
            {
                break;
            }
            else if ((black_Occ >> j) & 1)
            {
                mvs.push_back(move(sqr, j, 0));
                break;
            }
            else
            {
                mvs.push_back(move(sqr, j, 0));
            }
        }
        else
        {
            if ((black_Occ >> j) & 1)
            {
                break;
            }
            else if ((white_Occ >> j) & 1)
            {
                mvs.push_back(move(sqr, j, 0));
                break;
            }
            else
            {
                mvs.push_back(move(sqr, j, 0));
            }
        }

        j -= 8;
    }

    int r = rank(sqr);
    j = sqr;
    j++;
    while (rank(j) == r && squareOK(j))
    {
        if (side == true)
        {
            if ((white_Occ >> j) & 1)
            {
                break;
            }
            else if ((black_Occ >> j) & 1)
            {
                mvs.push_back(move(sqr, j, 0));
                break;
            }
            else
            {
                mvs.push_back(move(sqr, j, 0));
            }
        }
        else
        {
            if ((black_Occ >> j) & 1)
            {
                break;
            }
            else if ((white_Occ >> j) & 1)
            {
                mvs.push_back(move(sqr, j, 0));
                break;
            }
            else
            {
                mvs.push_back(move(sqr, j, 0));
            }
        }

        j++;
    }

    j = sqr;
    j--;
    while (rank(j) == r && squareOK(j))
    {
        if (side == true)
        {
            if ((white_Occ >> j) & 1)
            {
                break;
            }
            else if ((black_Occ >> j) & 1)
            {
                mvs.push_back(move(sqr, j, 0));
                break;
            }
            else
            {
                mvs.push_back(move(sqr, j, 0));
            }
        }
        else
        {
            if ((black_Occ >> j) & 1)
            {
                break;
            }
            else if ((white_Occ >> j) & 1)
            {
                mvs.push_back(move(sqr, j, 0));
                break;
            }
            else
            {
                mvs.push_back(move(sqr, j, 0));
            }
        }

        j--;
    }
}

void chessBoard::calcAttackSqrBishop(std::vector<move>& mvs, int sqr)
{
    int rk, fi, destSqr;
    for (int j = -1; j <= 1; j += 2)
    {
        for (int k = -1; k <= 1; k += 2)
        {
            for (int l = 1; l <= 7; l++)
            {
                rk = rank(sqr)+l*j;
                fi = file(sqr)+l*k;

                if ( !rankFileOK(rk) || !rankFileOK(fi) )
                    break;
                else
                {
                    destSqr = square(rk, fi);
                    if (side == true)
                    {
                        if ((white_Occ >> destSqr) & 1)
                        {
                            break;
                        }
                        else if ((black_Occ >> destSqr) & 1)
                        {
                            mvs.push_back(move(sqr, destSqr, 0));
                            break;
                        }
                        else
                        {
                            mvs.push_back(move(sqr, destSqr, 0));
                        }
                    }
                    else
                    {
                        if ((black_Occ >> destSqr) & 1)
                        {
                            break;
                        }
                        else if ((white_Occ >> destSqr) & 1)
                        {
                            mvs.push_back(move(sqr, destSqr, 0));
                            break;
                        }
                        else
                        {
                            mvs.push_back(move(sqr, destSqr, 0));
                        }
                    }
                }
            }
        }
    }
}

void chessBoard::calcAttackSqrQueen(std::vector<move>& mvs, int sqr)
{
    calcAttackSqrRook(mvs, sqr);
    calcAttackSqrBishop(mvs, sqr);
}

void chessBoard::calcAttackSqrKnight(std::vector<move>& mvs, int sqr)
{
    int destSqr;
    for (int j = -2; j <= 2; j += 4)
    {
        for (int k = -1; k <= 1; k += 2)
        {
            if ( rankFileOK(rank(sqr)+j) && rankFileOK(file(sqr)+k) )
            {
                destSqr = square(rank(sqr)+j, file(sqr)+k);
                if (side == true)
                {
                    if ( !((white_Occ >> destSqr) & 1) )
                    {
                        mvs.push_back(move(sqr, destSqr, 0));
                    }
                }
                else
                {
                    if ( !((black_Occ >> destSqr) & 1) )
                    {
                        mvs.push_back(move(sqr, destSqr, 0));
                    }
                }
            }

            if ( rankFileOK(rank(sqr)+k) && rankFileOK(file(sqr)+j) )
            {
                destSqr = square(rank(sqr)+k, file(sqr)+j);
                if (side == true)
                {
                    if ( !((white_Occ >> destSqr) & 1) )
                    {
                        mvs.push_back(move(sqr, destSqr, 0));
                    }
                }
                else
                {
                    if ( !((black_Occ >> destSqr) & 1) )
                    {
                        mvs.push_back(move(sqr, destSqr, 0));
                    }
                }
            }
        }
    }
}

void chessBoard::calcAttackSqrKing(std::vector<move>& mvs, int sqr)
{
    int destSqr;

    for (int j = -1; j <= 1; j += 1)
    {
        for (int k = -1; k <= 1; k += 1)
        {
            if(j==0 && k==0)
               continue;

            if ( rankFileOK(rank(sqr)+j) && rankFileOK(file(sqr)+k) )
            {
                destSqr = square(rank(sqr)+j, file(sqr)+k);

                if (side == true)
                {
                    if ( !((white_Occ >> destSqr) & 1) )
                    {
                        mvs.push_back(move(sqr, destSqr, 0));
                    }
                }
                else
                {
                    if ( !((black_Occ >> destSqr) & 1) )
                    {
                        mvs.push_back(move(sqr, destSqr, 0));
                    }
                }
            }
        }
    }

    if (side == true)
    {
        // castling
        U64 tmp = 0;
        if (castlRights[0] == 1)
        {
            tmp = 0;
            setBit(tmp, F1);
            setBit(tmp, G1);

            if ((occ & tmp) == false)
                // TODO: decide whether to do it here or later
                //&& !isSqrUnderAttack(E1, !board.side, board)
                //&& !isSqrUnderAttack(F1, !board.side, board))
            {
                mvs.push_back(move(E1, G1, CASTLING));
            }
        }

        if (castlRights[1] == 1)
        {
            tmp = 0;
            setBit(tmp, B1);
            setBit(tmp, C1);
            setBit(tmp, D1);
            if ((occ & tmp) == false)
                // TODO: same as above
                //&& !isSqrUnderAttack(E1, !board.side, board)
                //&& !isSqrUnderAttack(D1, !board.side, board))
            {
                mvs.push_back(move(E1, C1, CASTLING));
            }
        }
    }
    else
    {
        // castling
        U64 tmp = 0;
        if (castlRights[2] == 1)
        {
            tmp = 0;
            setBit(tmp, F8);
            setBit(tmp, G8);

            if ((occ & tmp) == false)
                // TODO: decide whether to do it here or later
                //&& !isSqrUnderAttack(E8, !board.side, board)
                //&& !isSqrUnderAttack(F8, !board.side, board))
            {
                mvs.push_back(move(E8, G8, CASTLING));
            }
        }

        if (castlRights[3] == 1)
        {
            tmp = 0;
            setBit(tmp, B8);
            setBit(tmp, C8);
            setBit(tmp, D8);
            if ((occ & tmp) == false)
                // TODO: same as above
                //&& !isSqrUnderAttack(E8, !board.side, board)
                //&& !isSqrUnderAttack(D8, !board.side, board))
            {
                mvs.push_back(move(E8, C8, CASTLING));
            }
        }
    }
}

void chessBoard::calcAttackSqrWhitePawn(std::vector<move>& mvs, int sqr)
{
    int destSqr;
    if ( rankFileOK(rank(sqr) + 1) && rankFileOK(file(sqr) + 1) )
    {
        destSqr = square(rank(sqr) + 1, file(sqr) + 1);
        if ((black_Occ >> destSqr) & 1)
        {
            if (rank(destSqr) == 7)
            {
                for (int i=PROMOTE_R; i<=PROMOTE_Q; i++)
                    mvs.push_back(move(sqr, destSqr, i));
            }
            else
                mvs.push_back(move(sqr, destSqr, 0));
        }

        if (destSqr == ep)
            mvs.push_back(move(sqr, destSqr, EP));
    }

    if ( rankFileOK(rank(sqr) + 1) && rankFileOK(file(sqr) - 1) )
    {
        destSqr = square(rank(sqr) + 1, file(sqr) - 1);
        if ((black_Occ >> destSqr) & 1)
        {
            if (rank(destSqr) == 7)
            {
                for (int i=PROMOTE_R; i<=PROMOTE_Q; i++)
                    mvs.push_back(move(sqr, destSqr, i));
            }
            else
                mvs.push_back(move(sqr, destSqr, 0));
        }

        if (destSqr == ep)
            mvs.push_back(move(sqr, destSqr, EP));
    }
}

void chessBoard::calcAttackSqrBlackPawn(std::vector<move>& mvs, int sqr)
{
    int destSqr;
    if ( rankFileOK(rank(sqr) - 1) && rankFileOK(file(sqr) + 1) )
    {
        destSqr = square(rank(sqr) - 1, file(sqr) + 1);
        if ((white_Occ >> destSqr) & 1)
        {
            if (rank(destSqr) == 0)
            {
                for (int i=PROMOTE_R; i<=PROMOTE_Q; i++)
                    mvs.push_back(move(sqr, destSqr, i));
            }
            else
                mvs.push_back(move(sqr, destSqr, 0));
        }

        if (destSqr == ep)
            mvs.push_back(move(sqr, destSqr, EP));
    }

    if ( rankFileOK(rank(sqr) - 1) && rankFileOK(file(sqr) - 1) )
    {
        destSqr = square(rank(sqr) - 1, file(sqr) - 1);
        if ((white_Occ >> destSqr) & 1)
        {
            if (rank(destSqr) == 0)
            {
                for (int i=PROMOTE_R; i<=PROMOTE_Q; i++)
                    mvs.push_back(move(sqr, destSqr, i));
            }
            else
                mvs.push_back(move(sqr, destSqr, 0));
        }

        if (destSqr == ep)
            mvs.push_back(move(sqr, destSqr, EP));
    }
}

void chessBoard::calcMoveSqrWhitePawn(std::vector<move>& mvs, int sqr)
{
    // TODO could add some checks
    //if ( rankFileOK(rank(sqr) + 1) )

    int destSqr = square(rank(sqr) + 1, file(sqr));
    // TODO: read about if 'if + return' advisable
    if ((occ >> destSqr) & 1)
        return;

    if (rank(destSqr) == 7)
    {
        for (int i=PROMOTE_R; i<=PROMOTE_Q; i++)
            mvs.push_back(move(sqr, destSqr, i));
    }
    else
        mvs.push_back(move(sqr, destSqr, 0));

    if (rank(sqr) == 1)
    {
        destSqr = square(3, file(sqr));
        if ( !((occ >> destSqr) & 1) )
            mvs.push_back(move(sqr, destSqr, 0));
    }
}

void chessBoard::calcMoveSqrBlackPawn(std::vector<move>& mvs, int sqr)
{
    // TODO could add some checks
    //if ( rankFileOK(rank(sqr) - 1) )

    int destSqr = square(rank(sqr) - 1, file(sqr));
    // TODO: read about if 'if + return' advisable
    if ((occ >> destSqr) & 1)
        return;

    if (rank(destSqr) == 0)
    {
        for (int i=PROMOTE_R; i<=PROMOTE_Q; i++)
            mvs.push_back(move(sqr, destSqr, i));
    }
    else
        mvs.push_back(move(sqr, destSqr, 0));

    if (rank(sqr) == 6)
    {
        destSqr = square(4, file(sqr));
        if ( !((occ >> destSqr) & 1) )
            mvs.push_back(move(sqr, destSqr, 0));
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

// TODO needs to be for both sides, optimized and tested!!
/*bool isSqrUnderAttack(int sqr, bool side, chessBoard& board)
{
    std::vector<int>::iterator it;
    U64 sqrOnMap = 0;
    setBit(sqrOnMap, sqr);

    if (side == true)
    {
        for (it = board.wRook.begin(); it != board.wRook.end(); it++)
        {
            U64 att = calcAttackSqrRook(*it, board.occ);
            if (att & sqrOnMap)
                return true;
        }

        for (it = board.wBishop.begin(); it != board.wBishop.end(); it++)
        {
            U64 att = calcAttackSqrBishop(*it, board.occ);
            if (att & sqrOnMap)
                return true;
        }

        for (it = board.wQueen.begin(); it != board.wQueen.end(); it++)
        {
            U64 att = calcAttackSqrQueen(*it, board.occ);
            if (att & sqrOnMap)
                return true;
        }

        for (it = board.wKnight.begin(); it != board.wKnight.end(); it++)
        {
            U64 att = calcAttackSqrKnight(*it);
            if (att & sqrOnMap)
                return true;
        }

        for (it = board.wKing.begin(); it != board.wKing.end(); it++)
        {
            U64 att = calcAttackSqrKing(*it);
            if (att & sqrOnMap)
                return true;
        }

        for (it = board.wPawn.begin(); it != board.wPawn.end(); it++)
        {
            if (sqr == board.ep)
                return true;

            U64 att = calcAttackSqrWhitePawn(*it);
            if (att & sqrOnMap)
                return true;
        }
    }
    else
    {
        for (it = board.bRook.begin(); it != board.bRook.end(); it++)
        {
            U64 att = calcAttackSqrRook(*it, board.occ);
            if (att & sqrOnMap)
                return true;
        }

        for (it = board.bBishop.begin(); it != board.bBishop.end(); it++)
        {
            U64 att = calcAttackSqrBishop(*it, board.occ);
            if (att & sqrOnMap)
                return true;
        }

        for (it = board.bQueen.begin(); it != board.bQueen.end(); it++)
        {
            U64 att = calcAttackSqrQueen(*it, board.occ);
            if (att & sqrOnMap)
                return true;
        }

        for (it = board.bKnight.begin(); it != board.bKnight.end(); it++)
        {
            U64 att = calcAttackSqrKnight(*it);
            if (att & sqrOnMap)
                return true;
        }

        for (it = board.bKing.begin(); it != board.bKing.end(); it++)
        {
            U64 att = calcAttackSqrKing(*it);
            if (att & sqrOnMap)
                return true;
        }

        for (it = board.bPawn.begin(); it != board.bPawn.end(); it++)
        {
            if (sqr == board.ep)
                return true;

            U64 att = calcAttackSqrBlackPawn(*it);
            if (att & sqrOnMap)
                return true;
        }
    }

    return false;
}*/

void chessBoard::genMoves(std::vector<move>& moves)
{
    for (int i=A1; i<=H8; i++)
    {
        if (side == true && charBoard[i] == 'R')
        {
            calcAttackSqrRook(moves, i);
        }
        else if (side == true && charBoard[i] == 'N')
        {
            calcAttackSqrKnight(moves, i);
        }
        else if (side == true && charBoard[i] == 'B')
        {
            calcAttackSqrBishop(moves, i);
        }
        else if (side == true && charBoard[i] == 'Q')
        {
            calcAttackSqrQueen(moves, i);
        }
        else if (side == true && charBoard[i] == 'K')
        {
            calcAttackSqrKing(moves, i);
        }
        else if (side == true && charBoard[i] == 'P')
        {
            calcMoveSqrWhitePawn(moves, i);
            calcAttackSqrWhitePawn(moves, i);
        }
        else if (side == false && charBoard[i] == 'r')
        {
            calcAttackSqrRook(moves, i);
        }
        else if (side == false && charBoard[i] == 'n')
        {
            calcAttackSqrKnight(moves, i);
        }
        else if (side == false && charBoard[i] == 'b')
        {
            calcAttackSqrBishop(moves, i);
        }
        else if (side == false && charBoard[i] == 'q')
        {
            calcAttackSqrQueen(moves, i);
        }
        else if (side == false && charBoard[i] == 'k')
        {
            calcAttackSqrKing(moves, i);
        }
        else if (side == false && charBoard[i] == 'p')
        {
            calcMoveSqrBlackPawn(moves, i);
            calcAttackSqrBlackPawn(moves, i);
        }
    }

    //std::vector<move>::iterator mvsIt;
    //for (mvsIt = moves.begin(); mvsIt != moves.end(); mvsIt++)
    //    std::cout << "From " << sqrMapIntToStr[(int)(*mvsIt).from]
    //              << " To "  << sqrMapIntToStr[(int)(*mvsIt).to] << std::endl;
}

U64* chessBoard::getBitBoard(char c)
{
    // TODO: can do sanity check for c here 

    U64* ret=0;
    switch (c)
    {
        case 'R' :
            ret = &wRook_Occ;     
        break;
        case 'N' :
            ret = &wKnight_Occ;     
        break;
        case 'B' :
            ret = &wBishop_Occ;     
        break;
        case 'Q' :
            ret = &wQueen_Occ;     
        break;
        case 'K' :
            ret = &wKing_Occ;     
        break;
        case 'P' :
            ret = &wPawn_Occ;     
        break;

        case 'r' :
            ret = &bRook_Occ;     
        break;
        case 'n' :
            ret = &bKnight_Occ;     
        break;
        case 'b' :
            ret = &bBishop_Occ;     
        break;
        case 'q' :
            ret = &bQueen_Occ;     
        break;
        case 'k' :
            ret = &bKing_Occ;     
        break;
        case 'p' :
            ret = &bPawn_Occ;     
        break;

        default:
        break;
    }

    return ret;
}

bool chessBoard::makeMove(move mv){

    int from = (int)mv.from, to = (int)mv.to;

    if (charBoard[from] == 'P' || charBoard[to] != ' ')
        fifty = 0;
    else
        fifty++;

    U64 tmp = 0;
    setBit(tmp, from);
    tmp = ~tmp;

    U64* updt = getBitBoard(charBoard[from]);
    (*updt) &= tmp;
    setBit(*updt, to);

    updt = &occ;
    (*updt) &= tmp;
    setBit(*updt, to);

    if (side == true)
        updt = &white_Occ;
    else
        updt = &black_Occ;
    (*updt) &= tmp;
    setBit(*updt, to);

    if (mv.flag == EP)
    {
       if (side == true)
       {
           tmp = 0;
           setBit(tmp, to-8);
           tmp = ~tmp;

           updt = &bPawn_Occ;
           (*updt) &= tmp;

           updt = &black_Occ;
           (*updt) &= tmp;

           updt = &occ;
           (*updt) &= tmp;
       }
       else
       {
           tmp = 0;
           setBit(tmp, to+8);
           tmp = ~tmp;

           updt = &wPawn_Occ;
           (*updt) &= tmp;

           updt = &white_Occ;
           (*updt) &= tmp;

           updt = &occ;
           (*updt) &= tmp;
       }
    }
    else if (mv.flag == CASTLING)
    {
        int rookFrom = 0, rookTo = 0;
        if (to == C1)
        { 
            rookFrom = A1, rookTo = D1;
        }
        else if (to == G1)
        {
            rookFrom = H1, rookTo = F1;
        }
        else if (to == C8)
        {
            rookFrom = A8, rookTo = D8;
        }
        else if (to == G8)
        {
            rookFrom = H8, rookTo = F8;
        }

        tmp = 0;
        setBit(tmp, rookFrom);
        tmp = ~tmp;

        if (side == true)
        {
            updt =  &wRook_Occ;
            (*updt) &= tmp;
            setBit(*updt, rookTo);

            updt = &white_Occ;
            (*updt) &= tmp;
            setBit(*updt, rookTo);
        }
        else
        {
            updt =  &bRook_Occ;
            (*updt) &= tmp;
            setBit(*updt, rookTo);

            updt = &black_Occ;
            (*updt) &= tmp;
            setBit(*updt, rookTo);
        }

        updt = &occ;
        (*updt) &= tmp;
        setBit(*updt, rookTo);
    }
    else
    {
        if (charBoard[to] != ' ')
        {
            tmp = 0;
            setBit(tmp, to);
            tmp = ~tmp;

            updt = getBitBoard(charBoard[to]);
            (*updt) &= tmp;

            if (side == true)
            {
                updt = &black_Occ;
                (*updt) &= tmp;
            }
            else
            {
                updt = &white_Occ;
                (*updt) &= tmp;
            }
        }
    }

    if (mv.flag == PROMOTE_R || mv.flag == PROMOTE_N
        || mv.flag == PROMOTE_B  || mv.flag == PROMOTE_Q )
    {
        tmp = 0;
        setBit(tmp, to);
        tmp = ~tmp;

        if (side == true)
            wPawn_Occ &= tmp;
        else
            bPawn_Occ &= tmp;

        if (side == true)
        {
            if (mv.flag == PROMOTE_R)
                updt = &wRook_Occ;
            else if (mv.flag == PROMOTE_N)
                updt = &wKnight_Occ;
            else if (mv.flag == PROMOTE_B)
                updt = &wBishop_Occ;
            else if (mv.flag == PROMOTE_Q)
                updt = &wQueen_Occ;
        }
        else
        {
            if (mv.flag == PROMOTE_R)
                updt = &bRook_Occ;
            else if (mv.flag == PROMOTE_N)
                updt = &bKnight_Occ;
            else if (mv.flag == PROMOTE_B)
                updt = &bBishop_Occ;
            else if (mv.flag == PROMOTE_Q)
                updt = &bQueen_Occ;
        }
        setBit(*updt, to);
    }

    charBoard[to] = charBoard[from];
    charBoard[from] = ' ';

    if (mv.flag == EP)
    {
       if (side == true)
           charBoard[to-8] = ' ';
       else
           charBoard[to+8] = ' ';
    }
    else if (mv.flag == CASTLING)
    {
        if (to == C1)
        {
            charBoard[A1] = ' '; charBoard[D1] = 'R';
        }
        else if (to == G1)
        {
            charBoard[H1] = ' '; charBoard[F1] = 'R';
        }
        else if (to == C8)
        {
            charBoard[A8] = ' '; charBoard[D8] = 'r';
        }
        else if (to == G8)
        {
            charBoard[H8] = ' '; charBoard[F8] = 'r';
        }
    }
    else if (mv.flag == PROMOTE_R)
    {
        if (side == true)
            charBoard[to] = 'R';
        else
            charBoard[to] = 'r';
    }
    else if (mv.flag == PROMOTE_N)
    {
        if (side == true)
            charBoard[to] = 'N';
        else
            charBoard[to] = 'n';
    }
    else if (mv.flag == PROMOTE_B)
    {
        if (side == true)
            charBoard[to] = 'B';
        else
            charBoard[to] = 'b';
    }
    else if (mv.flag == PROMOTE_Q)
    {
        if (side == true)
            charBoard[to] = 'Q';
        else
            charBoard[to] = 'q';
    }

    if (charBoard[to] == 'P' && to - from == 16)
        ep = to - 8;
    else if (charBoard[to] == 'p' && to - from == -16)
        ep = to + 8;
    else
        ep = -1;

    // TODO: can probably make use of side here
    if (charBoard[to] == 'K')
        castlRights[0] = castlRights[1] = false;
    else if (charBoard[to] == 'k')
        castlRights[2] = castlRights[3] = false;

    // TODO: simplify it using charBoard
    if (castlRights[0] == true && (from == H1 || to == H1))
        castlRights[0] = false;
    else if (castlRights[1] == true && (from == A1 || to == A1))
        castlRights[1] = false;

    if (castlRights[2] == true && (from == H8 || to == H8))
        castlRights[2] = false;
    else if (castlRights[3] == true && (from == A8 || to == A8))
        castlRights[3] = false;

    // TODO for clarity can move it to the bottom
    side = !side;

    bool ret = true;

    // TODO could use isInCheck proc here
    int sqrKing = squareWithKing(!side);
    std::vector<move> moves; std::vector<move>::iterator it;
    genMoves(moves);
    for (it = moves.begin(); it != moves.end(); it++)
    {
        if ((int)it->to == sqrKing)
        {
            ret = false;
            break;
        }
        else if (mv.flag == CASTLING)
        {
            // TODO: a bit of a hack with pawns below so should think of a better way of doing it 
            if ((int)mv.to == C1 &&
                ( (int)it->to == E1 || (int)it->to == D1 ||
                  charBoard[C2] == 'p' || charBoard[D2] == 'p' || charBoard[E2] == 'p' || charBoard[F2] == 'p' ) )
            {
                ret = false;
                break;
            }
            else if ((int)mv.to == G1 && ( (int)it->to == E1 || (int)it->to == F1 ||
                  charBoard[G2] == 'p' || charBoard[D2] == 'p' || charBoard[E2] == 'p' || charBoard[F2] == 'p' ) )
            {
                ret = false;
                break;
            }
            else if ((int)mv.to == C8 && ( (int)it->to == E8 || (int)it->to == D8 ||
                 charBoard[C7] == 'P' || charBoard[D7] == 'P' || charBoard[E7] == 'P' || charBoard[F7] == 'P' ) )
            {
                ret = false;
                break;
            }
            else if ((int)mv.to == G8 && ( (int)it->to == E8 || (int)it->to == F8 ||
                  charBoard[G7] == 'P' || charBoard[D7] == 'P' || charBoard[E7] == 'P' || charBoard[F7] == 'P' ) )
            {
                ret = false;
                break;
            }
        }
    }
    
    calcHash();

    return ret;
}

//TODO: remove parameter mv and commented out stuff
U64 Perft(chessBoard& board, int depth/*, move& mv*/)
{
    /*if (depth == 1)
    {
        std::cout << "---------------------------------------------------------------" << std::endl;
        std::cout << sqrMapIntToStr[(int)mv.from] << " " << sqrMapIntToStr[(int)mv.to];
    }*/

    std::vector<move> moves; std::vector<move>::iterator it;
    chessBoard boardCpy;
    U64 nodes = 0;
 
    if (depth == 0) 
        return 1;

    board.genMoves(moves);

    for (it = moves.begin(); it != moves.end(); it++)
    {
        boardCpy = board;

        if (board.makeMove(*it))
        {
            int tmp = Perft(board, depth - 1/*, *it*/);

            /*if (depth == 1)
                std::cout << sqrMapIntToStr[(int)(*it).from] << " "
                          << sqrMapIntToStr[(int)(*it).to]   << " "
                          << tmp << std::endl;*/

            nodes += tmp;
        }

        board = boardCpy;
    }
    //if (depth == 1)
    //    std::cout << " " << nodes << std::endl;

    return nodes;
}

struct debugItem {
    std::string pos;
    std::vector <U64> perftResults;
    debugItem(const std::string& p, const std::vector<U64>& perftRes)
             : pos(p) , perftResults(perftRes)
    { }

};

void debugMoveGen () {

    std::vector<debugItem> debugInput;

    debugInput.push_back(debugItem("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1",
                 {20, 400, 8902, 197281 }));
    /*debugInput.push_back(debugItem("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1",
                 {20, 400, 8902, 197281, 4865609, 119060324 }));
    debugInput.push_back(debugItem("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq -",
                 { 48, 2039, 97862, 4085603, 193690690 }));
    debugInput.push_back(debugItem("8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - -",
                 { 14, 191, 2812, 43238, 674624, 11030083, 178633661 }));
    debugInput.push_back(debugItem("r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1",
                 { 6, 264, 9467, 422333, 15833292, 706045033 }));
    debugInput.push_back(debugItem("rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8 ",
                 { 44, 1486, 62379, 2103487, 89941194 }));
    debugInput.push_back(debugItem("r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10",
                 { 46, 2079, 89890, 3894594, 164075551 }));*/

    std::vector<debugItem>::iterator it;
    for (it = debugInput.begin(); it != debugInput.end(); it++)
    {
        chessBoard board;

        readInPosFromFEN (board, it->pos);
        board.populateOccup();
        board.populateCharBoard();
        //printBoard2 (board.occ);
        //board.printCharBoard();

        int max = (it->perftResults).size();
        for (int i = 0; i < max; i++)
        {
            clock_t begin = clock();

            U64 tmp = Perft(board, i+1);

            if(tmp != (it->perftResults)[i])
                std::cout << "perft failed for position: " << it->pos << " at depth " << i+1
                          << " ,should be " << (it->perftResults)[i]
                          << " but returned " << tmp << std::endl;

            clock_t end = clock();
            double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
            std::cout << "spent " << elapsed_secs << " for " << it->pos
                      << " at depth " << i+1 << std::endl;
        }
    }
}

