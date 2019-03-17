#include <bitset>
#include <vector>
#include <chrono>

typedef unsigned long long U64;

// TODO: if use char here then need to convert char to int and vice-versa
// is it good solution ?
struct move {
    char from, to, flag;
    move(char f, char t, char fl)
        : from(f), to(t), flag(fl)  
    { }
};

// TODO: this caused a bug due to the order
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


// TODO: some more rules to add e.g. 50 moves rule
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
    bool checkOccup () {

    }

    void calcAttackSqrRook(std::vector<move>& mvs, int sqr);
    void calcAttackSqrBishop(std::vector<move>& mvs, int sqr);
    void calcAttackSqrQueen(std::vector<move>& mvs, int sqr);
    void calcAttackSqrKnight(std::vector<move>& mvs, int sqr);
    void calcAttackSqrKing(std::vector<move>& mvs, int sqr);
    void calcAttackSqrWhitePawn(std::vector<move>& mvs, int sqr);
    void calcAttackSqrBlackPawn(std::vector<move>& mvs, int sqr);
    void calcMoveSqrBlackPawn(std::vector<move>& mvs, int sqr);
    void calcMoveSqrWhitePawn(std::vector<move>& mvs, int sqr);

    void genMoves(std::vector<move>& moves);
    bool makeMove(move mv);

    int squareWithKing(bool sideToFind);
    bool isInCheck(bool sideToFind);
};

struct searchEngine {
    std::chrono::time_point<std::chrono::system_clock> startTime;
    double timeLimit;
    bool stopSearch;

    move doSearch(chessBoard& board, int depth);
    move searchMain(chessBoard& board, int depth);
    int search(chessBoard& board, int depth, int alpha, int beta);
    int quiesce(chessBoard& board, int alpha, int beta );
};

int eval(const chessBoard& board);

bool savePosInFEN(chessBoard& board, std::string& pos);
bool readInPosFromFEN(chessBoard& board, std::string pos);
void debugMoveGen ();


