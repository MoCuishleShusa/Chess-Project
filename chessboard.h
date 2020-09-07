#ifndef _CHESSB_H
#define _CHESSB_H
#include <vector>
#include <iostream>
#include "state.h"
#include "observer.h"
#include "subject.h"

class Chess;
class TextDisplay;
class GraphicsDisplay;

class Chessboard: public Subject<State> {
	std::vector<std::vector<Chess *>> board;
	TextDisplay *td = nullptr;
	GraphicsDisplay *gd = nullptr;
	std::vector<int> KingPos;
	public:
	~Chessboard();

	void init();
	void move(int, int, int, int); 
	void ActualMove(int, int, int, int, Colour, bool);
	bool CanMove(int, int, int, int, Colour); //For human player, check whether it is a valid move, if so, move, if not, return false
	bool CanMove(Colour); //For computer player, check whether a chess which can move, if so, store the start position and end position in the state
	bool CanCapture(Colour); //For computer player, check whether a chess can capture an enemies' chess, if so, store the infomation in the state
	bool CanAvoidCapture(Colour); //For computer player, check whether a chess move to another place or capture another chess and can avoid capture 
	bool MoreComplex(Colour, bool &); 
	bool IsStalemate(Colour);
	void Setup(Colour&);
	bool IsEmpty(int, int) const; // check whether a position in the chessboard is empty
        void MoveBack(State s);
        void resign();
	friend std::ostream &operator<<(std::ostream &out, const Chessboard &c);

	private:
	bool CanArrive(int c1, int r1, int c2, int r2) const;// check whether the chess in (c1, r1) can arrive (c2, r2)
	bool ValidMove(int c1, int r1, int c2, int r2); //check opposite colour, valid direction and no chess blocking between (c1, r1) and (c2, r2)
	bool IsInCheck(int c, int r); //check whether the chess in (c, r) would be captured in next step
	bool Castling(int, int, int);
	bool EnPassent(int, int, int, bool needcin = 1);
	bool WhiteWins();
	bool BlackWins();
	bool InCheck(int, int, int, int, bool highLevel = 0);
	void changeBoard(std::string, int, int);
	void ConPawnRep(int, int, std::string);
	bool checkDone();
};
#endif



