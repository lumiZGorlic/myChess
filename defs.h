#include <bitset>
#include <vector>
#include <chrono>
#include <set>
#include <map>

#define MAX_NUM_OF_MVS      250

enum { EP=1, CASTLING, PROMOTE_R, PROMOTE_N, PROMOTE_B, PROMOTE_Q };

enum {
   A1, B1, C1, D1, E1, F1, G1, H1,
   A2, B2, C2, D2, E2, F2, G2, H2,
   A3, B3, C3, D3, E3, F3, G3, H3,
   A4, B4, C4, D4, E4, F4, G4, H4,
   A5, B5, C5, D5, E5, F5, G5, H5,
   A6, B6, C6, D6, E6, F6, G6, H6,
   A7, B7, C7, D7, E7, F7, G7, H7,
   A8, B8, C8, D8, E8, F8, G8, H8
};


typedef unsigned long long U64;


struct pieceValue {

    std::map<char,int> values;

    int lookupValue(char p){
        // assert
        return values[p];
    }

    pieceValue() {
        values['R']=5; values['r']=5;
        values['B']=3; values['b']=3;
        values['N']=3; values['n']=3;
        values['Q']=9; values['q']=9;
        values['P']=1; values['p']=1;
        values['K']=10; values['k']=10;
    }
};


struct magics {

    /*

    EXPL:
    occupLines and occupDiags contain all the possible occupancy arrays
    excluding the border squares, so for 4x4 occupLines[4] will have the
    following ('x' signifies the staring square and is set to '0'):

      0 0 0 0
      1 0 0 0
      x 0 0 0
      0 0 0 0

      0 0 0 0
      1 0 0 0
      x 1 0 0
      0 0 0 0

      0 0 0 0
      1 0 0 0
      x 1 1 0
      0 0 0 0

      0 0 0 0
      1 0 0 0
      x 0 1 0
      0 0 0 0

      0 0 0 0
      0 0 0 0
      x 0 0 0
      0 0 0 0

      0 0 0 0
      0 0 0 0
      x 1 0 0
      0 0 0 0

      0 0 0 0
      0 0 0 0
      x 1 1 0
      0 0 0 0

      0 0 0 0
      0 0 0 0
      x 0 1 0
      0 0 0 0

    */
    std::set<U64> occupLines[64];
    std::set<U64> occupDiags[64];


    U64 diagNW[64], diagNE[64], diagSW[64], diagSE[64];
    U64 lineN[64],  lineS[64],  lineW[64],  lineE[64];


    int RBits[64] = {
      12, 11, 11, 11, 11, 11, 11, 12,
      11, 10, 10, 10, 10, 10, 10, 11,
      11, 10, 10, 10, 10, 10, 10, 11,
      11, 10, 10, 10, 10, 10, 10, 11,
      11, 10, 10, 10, 10, 10, 10, 11,
      11, 10, 10, 10, 10, 10, 10, 11,
      11, 10, 10, 10, 10, 10, 10, 11,
      12, 11, 11, 11, 11, 11, 11, 12
    };

    U64 rookMask[64];
    U64 rookAttackArr[64][4096];
    U64 rookMagics[64];


    int BBits[64] = {
      6, 5, 5, 5, 5, 5, 5, 6,
      5, 5, 5, 5, 5, 5, 5, 5,
      5, 5, 7, 7, 7, 7, 5, 5,
      5, 5, 7, 9, 9, 7, 5, 5,
      5, 5, 7, 9, 9, 7, 5, 5,
      5, 5, 7, 7, 7, 7, 5, 5,
      5, 5, 5, 5, 5, 5, 5, 5,
      6, 5, 5, 5, 5, 5, 5, 6
    };

    U64 bishopMask[64];
    U64 bishopAttackArr[64][4096];
    U64 bishopMagics[64];


    U64 knightAttackArr[64];
    U64 kingAttackArr[64];
    U64 wPawnAttackArr[64];
    U64 bPawnAttackArr[64];


    magics();

    void saveInFile();
    void readFromFile();

    U64 random_U64();
    U64 random_U64_fewbits();
    int count_1s(U64 b);
    U64 rmask(int sq);
    U64 bmask(int sq);

    void genOccForLinesNDiags();
    U64 makeMaskHoriz(int sqr);
    U64 makeMaskVertic(int sqr);
    U64 makeMaskS(int sqr);
    U64 makeMaskN(int sqr);
    U64 makeMaskW(int sqr);
    U64 makeMaskE(int sqr);
    U64 makeMaskSW(int sqr);
    U64 makeMaskSE(int sqr);
    U64 makeMaskNW(int sqr);
    U64 makeMaskNE(int sqr);

    void populDiags();
    void populLines();
    int bitscanForward(U64 board);
    int bitscanReverse(U64 board);
    U64 doAttackNW (U64 occup, int sqr);
    U64 doAttackNE (U64 occup, int sqr);
    U64 doAttackSW (U64 occup, int sqr);
    U64 doAttackSE (U64 occup, int sqr);
    U64 doAttackN (U64 occup, int sqr);
    U64 doAttackE (U64 occup, int sqr);
    U64 doAttackS (U64 occup, int sqr);
    U64 doAttackW (U64 occup, int sqr);
    U64 rookAttack(int sqr, U64 occ);
    U64 bishopAttack(int sqr, U64 occ);

    void calcMagicsRook ();
    void calcMagicsBishop ();

    void calcAttackSqrKnight();
    void calcAttackSqrKing();
    void calcAttackSqrWPawn();
    void calcAttackSqrBPawn();

    void populRook();
    void populBishop();

};


extern magics mag;
extern pieceValue pValue;


// TODO: if use char here then need to convert char to int and vice-versa
// is it good solution ?
struct move {
    char from, to, flag;
    int score;
    move(char f, char t, char fl)
        : from(f), to(t), flag(fl), score(0)
    { }
    move()
        : from(0), to(0), flag(0), score(0)
    { }
};

struct chessBoard {

    U64 occ, white_Occ, black_Occ,
        wKing_Occ, wQueen_Occ, wRook_Occ, wBishop_Occ, wKnight_Occ, wPawn_Occ,
        bKing_Occ, bQueen_Occ, bRook_Occ, bBishop_Occ, bKnight_Occ, bPawn_Occ;

    // ' ' - empty square, 'k' -  black king, 'N' - white knight etc
    char charBoard[64];

    // true = white, false = black
    bool side;
    std::bitset<4> castlRights;
    int ep;
    int fifty;

    // TODO: is it enough ? what about castling etc ?
    unsigned long long int squaresHash[64][6][2];
    unsigned long long int sideHash;
    unsigned long long int epHash[64];

    unsigned long long int currPosHash;
    std::vector<unsigned long long int> historyHash;

    int numOfMvs;


    chessBoard();
    unsigned long long int genHash();

    void populateOccup ();
    U64* getBitBoard(char c);
    void populateCharBoardHelper(U64 b, char c);
    // TODO: should be inline ? also could be moved to readIn func so that
    // everyting is in one place
    void populateCharBoard();
    void printCharBoard ();
    void calcHash();
    int currPosRepeats();

    // TODO
    // check occupancy so that it makes sense, for debugging purposes
    /*bool checkOccup () {
    }*/

    void calcAttackSqrRook(move* mvs, int sqr);
    void calcAttackSqrBishop(move* mvs, int sqr);
    void calcAttackSqrQueen(move* mvs, int sqr);
    void calcAttackSqrKnight(move* mvs, int sqr);
    void calcAttackSqrKing(move* mvs, int sqr);
    void calcAttackSqrWhitePawn(move* mvs, int sqr);
    void calcAttackSqrBlackPawn(move* mvs, int sqr);
    void calcMoveSqrBlackPawn(move* mvs, int sqr);
    void calcMoveSqrWhitePawn(move* mvs, int sqr);

    void genAgMvsHelper(move* mvs, int sqr, U64 attackedSqrs);
    void genAggressiveMoves(move* mvs);

    void genMoves(move* mvs);
    bool makeMove(move mv);

    int squareWithKing(bool sideToFind);
    bool isUnderAttack(int sqr, bool sideAttacking);
    bool isInCheck(bool sideToFind);
};

struct searchEngine {
    std::chrono::time_point<std::chrono::system_clock> startTime;
    double timeLimit;
    unsigned long long int nodes;

    move doSearch(chessBoard& board, int depth);
    move searchMain(chessBoard& board, int depth);
    int search(chessBoard& board, int depth, int alpha, int beta);
    int quiesce(chessBoard& board, int alpha, int beta );
};


struct debugItem {
    std::string pos;
    std::vector <U64> perftResults;
    debugItem(const std::string& p, const std::vector<U64>& perftRes)
             : pos(p) , perftResults(perftRes)
    { }
};


int eval(const chessBoard& board);


int rank(int sqr);
int file(int sqr);
int square(int rank, int file);
bool rankFileOK(int s);
bool squareOK(int s);
void setBit(U64& b, int sqr);
void printBoard(const U64& b);
void printBoard2(const U64& b);

U64 Perft(chessBoard& board, int depth);
U64 Perft2();
void debugMoveGen ();

bool savePosInFEN(chessBoard& board, std::string& pos);
bool readInPosFromFEN(chessBoard& board, std::string pos);


