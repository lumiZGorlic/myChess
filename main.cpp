#include "defs.h"
#include "board.h"
#include "search.h"
#include "testUtils.h"
#include "uci.h"

#include <execinfo.h>
#include <unistd.h>
#include <signal.h>


void handler(int sig) {
  void *array[10];
  size_t size;

  // get void*'s for all entries on the stack
  size = backtrace(array, 10);

  // print out all the frames to stderr
  fprintf(stderr, "Error: signal %d:\n", sig);
  backtrace_symbols_fd(array, size, STDERR_FILENO);
  exit(1);
}


int main ()
{
    signal(SIGSEGV, handler);

    uciLoop();

    //bratkoKopec();
    //testSearch();
    //debugMoveGen();
    //testMoveGenSpeed();
    //playDummyGame();
    //engineVsEngine();
    //playChess();
    //perft2();


    // the below stuff from https://www.sparkchess.com/mate-in/two
    // can be used to check if the best combinations are found. Should create
    // a separate function for it

    //solveChessPuzzle("r5rk/2p1Nppp/3p3P/pp2p1P1/4P3/2qnPQK1/8/R6R w - - 1 0");
    //solveChessPuzzle("kbK5/pp6/1P6/8/8/8/8/R7 w - -");

    //solveChessPuzzle("r2qk2r/pb4pp/1n2Pb2/2B2Q2/p1p5/2P5/2B2PPP/RN2R1K1 w - - 1 0");
    //solveChessPuzzle("6k1/pp4p1/2p5/2bp4/8/P5Pb/1P3rrP/2BRRN1K b - - 0 1");
    //solveChessPuzzle("8/2k2p2/2b3p1/P1p1Np2/1p3b2/1P1K4/5r2/R3R3 b - - 0 1");
    //solveChessPuzzle("6k1/5p2/1p5p/p4Np1/5q2/Q6P/PPr5/3R3K w - - 1 0");
    //solveChessPuzzle("r1b2k1r/ppppq3/5N1p/4P2Q/4PP2/1B6/PP5P/n2K2R1 w - - 1 0");
    //solveChessPuzzle("r1b2k1r/ppp1bppp/8/1B1Q4/5q2/2P5/PPP2PPP/R3R1K1 w - - 1 0");
    //solveChessPuzzle("rn2kb1r/ppp1pppp/8/8/4q3/3P1N1b/PPP1BPnP/RNBQ1K1R b kq - 0 1");
    //solveChessPuzzle("kbK5/pp6/1P6/8/8/8/8/R7 w - -");
    //solveChessPuzzle("4r1k1/pQ3pp1/7p/4q3/4r3/P7/1P2nPPP/2BR1R1K b - - 0 1");
    //solveChessPuzzle("r4R2/1b2n1pp/p2Np1k1/1pn5/4pP1P/8/PPP1B1P1/2K4R w - - 1 0");
    //solveChessPuzzle("7r/p3ppk1/3p4/2p1P1Kp/2Pb4/3P1QPq/PP5P/R6R b - - 0 1");
    //solveChessPuzzle("4r3/pbpn2n1/1p1prp1k/8/2PP2PB/P5N1/2B2R1P/R5K1 w - - 1 0");

    /*
    "r2qk2r/pb4pp/1n2Pb2/2B2Q2/p1p5/2P5/2B2PPP/RN2R1K1 w - - 1 0"  1. Qg6+ hxg6 2. Bxg6#
    "6k1/pp4p1/2p5/2bp4/8/P5Pb/1P3rrP/2BRRN1K b - - 0 1"  1... Rg1+ 2. Kxg1 Rxf1#
    "8/2k2p2/2b3p1/P1p1Np2/1p3b2/1P1K4/5r2/R3R3 b - - 0 1" 1... Bb5+ 2. Nc4 Rd2#
    "6k1/5p2/1p5p/p4Np1/5q2/Q6P/PPr5/3R3K w - - 1 0"  1. Qf8+ Kxf8 2. Rd8#
    "r1b2k1r/ppppq3/5N1p/4P2Q/4PP2/1B6/PP5P/n2K2R1 w - - 1 0" 1. Qxh6+ Rxh6 2. Rg8#
    "r1b2k1r/ppp1bppp/8/1B1Q4/5q2/2P5/PPP2PPP/R3R1K1 w - - 1 0" 1. Qd8+ Bxd8 2. Re8#
    "rn2kb1r/ppp1pppp/8/8/4q3/3P1N1b/PPP1BPnP/RNBQ1K1R b kq - 0 1" 1... Nh4+ 2. Ke1 Nxf3#
    "kbK5/pp6/1P6/8/8/8/8/R7 w - -" 1. Ra6 bxa6 2. b7#
    "4r1k1/pQ3pp1/7p/4q3/4r3/P7/1P2nPPP/2BR1R1K b - - 0 1"  1... Qxh2+ 2. Kxh2 Rh4#
    "r4R2/1b2n1pp/p2Np1k1/1pn5/4pP1P/8/PPP1B1P1/2K4R w - - 1 0" 1. h5+ Kh6 2. Nf7#
    "7r/p3ppk1/3p4/2p1P1Kp/2Pb4/3P1QPq/PP5P/R6R b - - 0 1" 1... Be3+ 2. Qxe3 Qg4#
    "4r3/pbpn2n1/1p1prp1k/8/2PP2PB/P5N1/2B2R1P/R5K1 w - - 1 0" 1. Rxf6+ Nxf6 2. g5#

    */


    return 0;
}
