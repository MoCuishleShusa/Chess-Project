#ifndef STATE_H
#define STATE_H
#include <string>
enum class Colour { NoColour, Black, White };
enum class Direction{NW, N, NE, W, E, SW, S, SE,
                     K1, K2, K3, K4, K5, K6, K7,K8};

struct State {
        public:
  bool IsCastling;
  bool IsEnPassant;
  int SetUp;// -1 is for removing, 1 is for adding, 2 is for turn colour, 
            // 3 is for replace, 4 is for undo, 5 is for undoReplace
            // 6 is for stalemate, 7 is for winner, 8 is for resign
  std::string chess;// moving chess
  int c1, r1, c2, r2, c3, r3, c4, r4; //(c1, r1) is the start position, 
                                      //(c2, r2) in the end position
                                      //same for (c3, r3) -> (c4, r4)
  std::string CapturedChess;// captured chess
  bool firstMoved;
  bool secMoved;
};

#endif

