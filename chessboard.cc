#include <vector>
#include <sstream>
#include <cstdlib>
#include <iostream>
#include <utility>
#include "chessboard.h"
#include "chess.h"
#include "state.h"
#include "textdisplay.h"
#include "graphicsdisplay.h"
using namespace std;
using std::string;

Chessboard::~Chessboard() {
	for (auto c: board) {
		for (auto v: c) delete v;
	}
	delete td; delete gd;
}

//Initialize the board
//No-throw guarantee
void Chessboard::init() {
	if (! board.empty()) { //clear the board
		for (auto &v: board) {
			for (auto c:v){
				delete c;
			}
		}
		board.clear();
		        board.clear();
			detach(td);
			detach(gd);
			KingPos.clear();
	}
	for(int col{0}; col < 8; col++) {
		vector<Chess *> chess;
		board.emplace_back(chess);
		for (int row{0}; row < 8; row++) { // initialize the board
			Chess *c = nullptr;
			if (((col == 0) || (col == 7)) && ((row == 0) || (row == 7))) {
				if (row == 0) c = new Rook{col, row, Colour::Black};
				else c = new Rook{col, row, Colour::White};
	
			} else if (((col == 1) || (col == 6)) && ((row == 0) || (row == 7))) {
				if (row == 0) c = new Knight{col, row, Colour::Black};
				else c = new Knight{col, row, Colour::White};
	
			} else if (((col == 2) || (col == 5)) && ((row == 0) || (row == 7))) {
				if (row == 0) c = new Bishop{col, row, Colour::Black};
				else c = new Bishop{col, row, Colour::White};
		
			} else if ((col == 3) && ((row == 0) || (row == 7))) {
				if (row == 0) c = new Queen{col, row, Colour::Black};
				else c = new Queen{col, row, Colour::White};
		
			} else if ((col == 4) && ((row == 0) || (row == 7))) {
				if (row == 0) c = new King{col, row, Colour::Black};
				else c = new King{col, row, Colour::White};
	
			} else if (row == 1) { c = new Pawn{col, row, Colour::Black};
			} else if (row == 6) { c = new Pawn{col, row, Colour::White};
			} else {}
			board.at(col).emplace_back(c);
		}
	}
	td = new TextDisplay{8};
	gd = new GraphicsDisplay{8};
	attach(td); attach(gd);
	KingPos.emplace_back(4); //black king col
	KingPos.emplace_back(0); //black king row
	KingPos.emplace_back(4); //white king col
	KingPos.emplace_back(7); //white king row
}

//Move the chess in (c1, r1) to (c2, r2)
//Require: (c1, r1) and (c2, r2) are valid
//No-throw guarantee
void Chessboard::move(int c1, int r1, int c2, int r2){ // actual move
	if (board[c1][r1]->getType() == "King") { //change KingPos if King is moved
		if (board[c1][r1]->getInfo().colour == Colour::Black) {
			KingPos[0] = c2; KingPos[1] = r2;
		} else {
			KingPos[2] = c2; KingPos[3] = r2;
		}
	}

	State s{getState()}; //for undo

	board.at(c2).at(r2) = board.at(c1).at(r1);
	(board.at(c2).at(r2))->setRow(r2);
	(board.at(c2).at(r2))->setCol(c2);
	board.at(c1).at(r1) = nullptr;
	State state{false, false, 0, board[c2][r2]->getType(), c1, r1, c2, r2, 
	  0, 0, 0, 0, s.CapturedChess, s.firstMoved, s.secMoved};
	setState(state);
}

//Consider castling, if it can do castling, then do castling and return true; if not, do nothing and return false
//Require: (c1, r) and (c2, r) are valid
//No-throw guarantee
bool Chessboard::Castling(int c1, int c2, int r) {
	if ((! IsEmpty(c2, r)) || (IsInCheck(c1, r)) || (board[c1][r]->getStatus())) return false;
	int SecPos; // the position between King (start position) and the end position
	int RookPos; // the position Rook should arrive
	if (c1 > c2) { // if move left
		SecPos = c1 - 1;
		for (int i{c1 - 3}; i >= 0; i--) {
			if ((!board[i][r]) && (i != 0)) continue;
			else if ((board[i][r]) && (board[i][r]->getType() == "Rook")) RookPos = i;
			else return false;
		}
	} else { // if move right
		SecPos = c1 + 1;
		for (int i{c1 + 3}; i <= 7; i++) {
			if ((!board[i][r]) && (i != 0)) continue;
			else if ((board[i][r]) && (board[i][r]->getType() == "Rook")) RookPos = i;
			else return false;
		}
	}
	if ((! IsEmpty(SecPos, r)) || (board[RookPos][r]->getStatus())) return false;
	if (! InCheck(c1, r, c2, r)) { // check moving from (c1, r) to (c2, r) would not cause King in check
		move(c1, r, c2, r);
		if (! InCheck(RookPos, r, SecPos, r)) { // check Rook skipped over King would not cause King in check
			move(RookPos, r, SecPos, r);
			board[c2][r]->setStatus(1);
			board[SecPos][r]->setStatus(1);
			State state{true, false, 0, "", c1, r, c2, r, RookPos, r, SecPos, r, "", false, false}; // for undo
			setState(state);
			notifyObservers();
			return true;
		} else {
			move(c2, r, c1, r); //move back if Rook skipping over King would cause King in check
		}
	}
	return false;
}

// Consider En Passant, if there is a pawn can en passant, then 
// ask the player whether to en passant and do what they want and return true
// if pawn in (c, r1) can move to (c, r2), return true
// if it is not valid to move from (c, r1) to (c, r2), then return false
// Require: (c, r1) and (c, r2) are valid
// No-throw guarantee
bool Chessboard::EnPassent(int c, int r1, int r2, bool needcin) { // if the player is human, needcin is true
	if (! IsEmpty(c, r2)) return false;
	if (board[c][r1]->getStatus()) return false;
	int SecPos; // the end position of the would-be attacking pawn
	char col; string pos; ostringstream ss;
	if (board[c][r1]->getInfo().colour == Colour::Black) {
		if (r1 - r2 == 2) return false;
		SecPos = r1 + 1;
	} else {
		if (r1 - r2 == -2) return false;
		SecPos = r1 - 1;
	}
	if (IsEmpty(c, SecPos)) {
		if (! InCheck(c, r1, c, r2)) { 
		// check whether moving from (c, r1) to (c, r2) would cause own King in check
		    move(c, r1, c, r2);
		    board[c][r2]->setStatus(1);
	            if ((c + 1 <= 7) && (! IsEmpty(c + 1, r2)) 
			&& (board[c + 1][r2]->getInfo().colour != board[c][r2]->getInfo().colour)
			&& (board[c + 1][r2]->getType() == "Pawn")) { 
			    // whether a would-be attacking pawn is in the left
			if (! InCheck(c + 1, r2, c, SecPos)) { 
				// check whether the would-be attacking pawn moving to the diagonal piece is valid
			    col = 'a' + c + 1; ss << col << 8 - r2; pos = ss.str();
		            string doornot; //whether en passant
			    if (needcin == 1) {
				cout << "Chess in ";
				cout <<	pos << " can capture a neighboring pawn." << endl;
				cout << "En Passant or not, ";
				cout << "please enter 'True' or 'False'." << endl;
				cin >> doornot;
			    } else doornot = "True";
		            if (doornot == "True") {
				delete board[c][r2];
				board[c][r2] = nullptr;
				bool SecMoved{board[c + 1][r2]->getStatus()}; // for undo
				move(c + 1, r2, c, SecPos);
				board[c][SecPos]->setStatus(1);
				State state{false, true, 0, "", c, r1, c, r2, 
					c + 1, r2, c, SecPos, "", false, SecMoved};
				setState(state);
				notifyObservers();
				return true;
			    }
			}
		   } else if ((c - 1 >= 0) && (! IsEmpty(c - 1, r2)) 
		        && (board[c - 1][r2]->getInfo().colour != board[c][r2]->getInfo().colour)
			&& (board[c - 1][r2]->getType() == "Pawn")) { 
			//a would-be attacking pawn is in the right
			if (! InCheck(c - 1, r2, c, SecPos)) {
			       col = 'a' + c - 1; ss << col << 8 - r2; pos = ss.str();
			       string doornot;
			       if (needcin == 1) {
				  cout << "Chess in " << pos;
				  cout << " can capture a neighboring pawn." << endl;
				  cout << "En Passant or not, ";
			          cout << "please enter 'True' or 'False'." << endl;
				  cin >> doornot;
			       } else doornot = "True";
			       if (doornot == "True") {
			  	  delete board[c][r2];
			  	  board[c][r2] = nullptr;
				  bool SecMoved{board[c - 1][r2]->getStatus()}; // for undo
		 		  move(c - 1, r2, c, SecPos);
			          board[c][SecPos]->setStatus(1);
				  State state{false, true, 0, "", c, r1, c, r2, 
				 	c - 1, r2, c, SecPos, "", false, SecMoved};
				  setState(state);
				  notifyObservers();
				  return true;
				}
			}
		}
	    // if there is no pawn at the corner to en passant 
	    // but moving from (c, r1) to (c, r2) is still valid
		State state{false, false, 0, "Pawn", c, r1, c, r2,
		       	0, 0, 0, 0, "", false, false};
		setState(state);
		notifyObservers();
		return true;
		}
	}
	return false;
}


// Consider pawn replacement, if both (c, r) and 
// chess type(s) are valid then points to a new chess
// Strong guarantee
void Chessboard::ConPawnRep(int c, int r, string s) {
	Chess *temp = board[c][r];
	Colour colour = board[c][r]->getInfo().colour;
	board[c][r] = nullptr;
	if (s == "N" && colour == Colour::White) {
		board[c][r] = new Knight{c, r, colour};
	} else if (s == "Q" && colour == Colour::White) {
		board[c][r] = new Queen{c, r, colour};
	} else if (s == "B" && colour == Colour::White) {
		board[c][r] = new Bishop{c, r, colour};
	} else if (s == "R" && colour == Colour::White) {
		board[c][r] = new Rook{c, r, colour};
	} else if (s == "n" && colour == Colour::Black) {
		board[c][r] = new Knight{c, r, colour};
	} else if (s == "q" && colour == Colour::Black) {
		board[c][r] = new Queen{c, r, colour};
	} else if (s == "b" && colour == Colour::Black) {
		board[c][r] = new Bishop{c, r, colour};
	} else if (s == "r" && colour == Colour::Black) {
		board[c][r] = new Rook{c, r, colour};
	} else { board[c][r] = temp; string ss{"Invalid replacement."}; throw ss; }
}

// Check whether (c1, r1) to (c2, r2) is a valid move, if so, move, if not, return false
// Require: (c1, r1) and (c2, r2) exist
// Basic guarantee
bool Chessboard::CanMove(int c1, int r1, int c2, int r2, Colour colour){
	if (board[c1][r1]->getInfo().colour != colour) return false;
	//consider castling
	if ((board[c1][r1]->getType() == "King") && ((c1 - c2 == 2) 
	    || (c1 - c2 == -2)) && (r1 == r2)) { return Castling(c1, c2, r1);}
	//consider en passant
	if ((board[c1][r1]->getType() == "Pawn") && ((r1 - r2 == 2) 
	    || (r1 - r2 == -2)) && (c1 == c2)) { return EnPassent(c1, r1, r2);}
	//check opposite colour, valid direction and no blocking in the line (except Knight)
	if (! ValidMove(c1, r1, c2, r2)) { return false;}
	//check own King is not in check
	if (InCheck(c1, r1, c2, r2)) { return false;}
	ActualMove(c1, r1, c2, r2, colour, true);
	return true;
}

//Actual move, change state and the status of chess (whether it 
//has moved already) if the pawn reach the other end of the board, 
//human player need to give new chess tyep name, that is, neednew is true
//Require: (c1, r1) and (c2, r2) are valid
//Strong guarantee
void Chessboard::ActualMove(int c1, int r1, int c2, int r2, Colour colour, bool neednew) {
	bool succeed{false};
	if (board[c1][r1]->getType() == "Pawn" 
	    && ((board[c1][r1]->getInfo().colour == Colour::Black && r2 == 7) 
	    || (board[c1][r1]->getInfo().colour == Colour::White && r2 == 0))) {
	    string c;
	    if (neednew == true) { 
		while (! (cin >> c)) { 
			cout << "Please replace the pawn by either";
			cout << "a rook, knight, bishop, or queen" << endl;}
	    } else { // if it is highlevel computer player
		if (colour == Colour::Black) c = "q";
		else c = "Q";
	    }
	    try{
		ConPawnRep(c1, r1, c);
            } // if c is not a valid chess type
		catch (string e) { throw e;}
		succeed = true;
	}
	
	//for undo
	string caped{""}; 
        bool firstMoved{false};
        bool secMoved{false};
        if (board[c2][r2]){
                caped = board[c2][r2]->getType();
                secMoved= board[c2][r2]->getStatus();
        }
        if (board[c1][r1]){
                firstMoved = board[c1][r1]->getStatus();
        }
	State myState{getState()};
	myState.CapturedChess = caped;
	myState.firstMoved = firstMoved;
	myState.secMoved = secMoved;
	setState(myState);

	delete board[c2][r2];
	board[c2][r2] = nullptr;
	move(c1, r1, c2, r2); //move 
	
	if (succeed) {State state{false, false, 3, board[c2][r2]->getType(), 
		c1, r1, c2, r2, 0, 0, 0, 0, caped, firstMoved, secMoved}; setState(state);}
	board[c2][r2]->setStatus(1); //turn IsMoved to be true
	notifyObservers(); //notify textdisplay and graphicsdisplay to change
	string s;

	if (BlackWins()) {
		cout << "Checkmate! Black wins!" << endl;
		State state{false, false, 7, "", 0, 0, 0, 0, 0, 0, 0, 0, "", 0, 0};
		setState(state);
		notifyObservers();
		s = "Black wins!"; throw s;
	} else if (WhiteWins()) {
		cout << "Checkmate! White wins!" << endl;
		State state{false, false, 7, "", 0, 0, 0, 0, 0, 0, 0, 0, "", 0, 0};
		setState(state);
		notifyObservers();
		s = "White wins!"; throw s;
	}
	if (colour == Colour::White) {
		if (IsInCheck(KingPos[0], KingPos[1])) 
			cout << "Black is in check!" << endl;
	} else {
		if (IsInCheck(KingPos[2], KingPos[3])) 
			cout << "White is in check!" << endl;
	}
}


// Try to find the next position in direction d, if succeed
// change the col and row, if not, do nothing and return false
// No-throw guatantee
static bool NextInDir(int &col, int &row, Direction d) {
	if (d == Direction::S) {
		if (row + 1 <= 7) { ++row; return true;}
	} else if (d == Direction::N) {
		if (row - 1 >= 0) { --row; return true;}
	} else if (d == Direction::W) {
		if (col - 1 >= 0) { --col; return true;}
	} else if (d == Direction::E) {
		if (col + 1 <= 7) { ++col; return true;}
	} else if (d == Direction::NW) {
		if ((col - 1 >= 0) && (row - 1 >= 0)) { 
			--col; --row; return true;}
	} else if (d == Direction::NE) {
		if ((col + 1 <= 7) && (row - 1 >= 0)) { 
			++col; --row; return true;}
	} else if (d == Direction::SE) {
		if ((col + 1 <= 7) && (row + 1 <= 7)) { 
			++col; ++row; return true;}
	} else if (d == Direction::K1) {
		if ((col + 2 <= 7) && (row + 1 <= 7)) { 
			col = col + 2; ++row; return true;}
	} else if (d == Direction::K2) {
		if ((col + 2 <= 7) && (row - 1 >= 0)) { 
			col = col + 2; --row; return true;}
	} else if (d == Direction::K3) {
		if ((col + 1 <= 7) && (row + 2 <= 7)) { 
			++col; row = row + 2; return true;}
	} else if (d == Direction::K4) {
		if ((col + 1 <= 7) && (row - 2 >= 0)) { 
			++col; row = row - 2; return true;}
	} else if (d == Direction::K5) {
		if ((col - 2 >= 0) && (row + 1 <= 7)) { 
			col = col - 2; ++row; return true;}
	} else if (d == Direction::K6) {
		if ((col - 2 >= 0) && (row - 1 >= 0)) { 
			col = col - 2; --row; return true;}
	} else if (d == Direction::K7) {
		if ((col - 1 >= 0) && (row + 2 <= 7)) { 
			--col; row = row + 2; return true;}
	} else if (d == Direction::K8) {
		if ((col - 1 >= 0) && (row - 2 >= 0)) { 
			--col; row = row - 2; return true;}
	} else if (d == Direction::SW) { 
		if ((col - 1 >= 0) && (row + 1 <= 7)) { 
			--col; ++row; return true;}
	}
	return false;
}


// Check whether a chess can move, if so, 
// store the start pos and end pos in the state
// if not, do nothing and return false
// No-throw guarantee
bool Chessboard::CanMove(Colour colour){
	vector<int> col;
	vector<int> row;
	for (size_t i{0}; i < 8; ++i) col.emplace_back(i);
	for (size_t i{0}; i < 8; ++i) row.emplace_back(i);
	// get random order of coloumn
	for (size_t i{7}; i > 0; --i) std::swap(col[i], col[rand()%i]);
	for (size_t i{0}; i < 8; ++i) {
	   // get random order of row
	   for (size_t h1{7}; h1 > 0; --h1) std::swap(row[h1], row[rand()%h1]);
	   for (size_t j{0}; j < 8; ++j) {
		Chess *v = board[col[i]][row[j]];
		if ((v) && v->getInfo().colour == colour) {
		   size_t size{v->Dir.size()};
	     	   vector<int> d;
		   //get random order of choosing direction
		   for (size_t i{0}; i < size; i++) d.emplace_back(i);
		   for (size_t i{size - 1}; i > 0; --i) std::swap(d[i], d[rand()%i]);
		   for (size_t i{0}; i < size; i++) {
		      int row{v->getInfo().row};
		      int col{v->getInfo().col};
		      if (NextInDir(col, row, v->Dir[d[i]])) {
			 // if there is a next valid chess in this direction
			 if ((ValidMove(v->getInfo().col, v->getInfo().row, col, row)) &&
			   (! InCheck(v->getInfo().col, v->getInfo().row, col, row))) {
				//for undo
				string caped{""};
				bool firstMoved{false};
				bool secMoved{false};
				if (board[col][row]){
					caped = (board[col][row]->getType());
					secMoved = (board[col][row]->getStatus());
				}
				if (board[v->getInfo().col][v->getInfo().row]){
				       	firstMoved = (board[v->getInfo().col][v->getInfo().row]->getStatus());
				}
				State state{false, false, 0, "", v->getInfo().col, v->getInfo().row, 
					col, row, 0, 0, 0, 0, caped, firstMoved, secMoved};
				setState(state);
				return true;
				}
		            }
	               }
	          }
	     }
	}
	return false;
}

// Check whether a chess can capture an enemies' chess, 
// or the next step can put opposite King in check
// if so, store the information in the state
// No-throw guarantee
bool Chessboard::CanCapture(Colour colour){
	vector<int> col;
	vector<int> row;
	for (size_t i{0}; i < 8; ++i) col.emplace_back(i);
	for (size_t i{0}; i < 8; ++i) row.emplace_back(i);
	// get random order of coloumn
	for (size_t i{7}; i > 0; --i) std::swap(col[i], col[rand()%i]);
	for (size_t i{0}; i < 8; ++i) {
	    // get random order of row
	   for (size_t h1{7}; h1 > 0; --h1) std::swap(row[h1], row[rand()%h1]);
           for (size_t j{0}; j < 8; ++j) {
		Chess *v = board[col[i]][row[j]];
		if ((v) && v->getInfo().colour == colour) {
		    size_t size{v->Dir.size()};
		    vector<int> d;
		     //get random order of choosing direction
	    	for (size_t i{0}; i < size; ++i) d.emplace_back(i);
		    for (size_t i{size - 1}; i > 0; --i) std::swap(d[i], d[rand()%i]);
		    for (size_t i{0}; i < size; ++i) {
			int row{v->getInfo().row};
			int col{v->getInfo().col};
			// check one direction until cannot move anymore
			while (NextInDir(col, row, v->Dir[d[i]])) {
			    // if this step can capture a chess
	   		  if ((board[col][row]) 
			    && (ValidMove(v->getInfo().col, v->getInfo().row, col, row)) 
				&& (! InCheck(v->getInfo().col, v->getInfo().row, col, row))) {
				string caped{""};   // for undo
                bool firstMoved{false};
                bool secMoved{false};
                if (board[col][row]){
                  caped = board[col][row]->getType();
                  secMoved= board[col][row]->getStatus();
                }
                if (board[v->getInfo().col][v->getInfo().row]){
                  firstMoved = board[v->getInfo().col][v->getInfo().row]->getStatus();
                }

				State state{false, false, 0, "", 
					v->getInfo().col, v->getInfo().row, col, row,
					0, 0, 0, 0,caped, firstMoved, secMoved};
				setState(state);
				return true;
			    } else if (ValidMove(v->getInfo().col, v->getInfo().row, col, row) &&
				(! InCheck(v->getInfo().col, v->getInfo().row, col, row))) {
				// if this step can put opposite King in check
				bool succeed{0};
				int c1 = v->getInfo().col; int r1 = v->getInfo().row;
				move(c1, r1, col, row);
				if (colour == Colour::White) {
					if (ValidMove(col, row, KingPos[0], KingPos[1])) succeed = 1;
				} else {
					if (ValidMove(col, row, KingPos[2], KingPos[3])) succeed = 1;
				}
  				move(col, row, c1, r1);
 		 		if (succeed) {
					string caped{""};   // for undo
                    bool firstMoved{false};
                    bool secMoved{false};
                    if (board[col][row]){
                        caped = board[col][row]->getType();
                        secMoved= board[col][row]->getStatus();
                    }
                    if (board[v->getInfo().col][v->getInfo().row]){
                        firstMoved = board[v->getInfo().col][v->getInfo().row]->getStatus();
                    }
					State state{false, false, 0, "", 
					v->getInfo().col, v->getInfo().row, col, row,
						0, 0, 0, 0, caped, firstMoved, secMoved};
					setState(state);
					return true;
				   }
				}
			    }
		        }
	            }
		}
	}
	return false;
}


// check whether a chess can avoid capture as well as
// capture another chess or put the opposite King in check
// if so, return true, store the information in the state
// if not, do nothing, return false
// No-throw guarantee
bool Chessboard::CanAvoidCapture(Colour colour){
	vector<int> col;
	vector<int> row;
	for (size_t i{0}; i < 8; ++i) col.emplace_back(i);
	for (size_t i{0}; i < 8; ++i) row.emplace_back(i);
	// get random order of coloumn
	for (size_t i{7}; i > 0; --i) std::swap(col[i], col[rand()%i]);
	for (size_t i{0}; i < 8; ++i) {
	    // get random order of row
	  for (size_t h1{7}; h1 > 0; --h1) std::swap(row[h1], row[rand()%h1]);
      for (size_t j{0}; j < 8; ++j) {
		Chess *v = board[col[i]][row[j]];
		if ((v) && v->getInfo().colour == colour) {
		    size_t size{v->Dir.size()};
		    vector<int> d;
	        //get random order of choosing direction
	        for (size_t i{0}; i < size; ++i) d.emplace_back(i);
	        for (size_t i{size - 1}; i > 0; --i) std::swap(d[i], d[rand()%i]);
	        for (size_t i{0}; i < size; ++i) {
			int row{v->getInfo().row};
			int col{v->getInfo().col};
			// check one direction until cannot move anymore
			while (NextInDir(col, row, v->Dir[d[i]])) {
			    if (IsInCheck(v->getInfo().col, v->getInfo().row) 
			     && (board[col][row]) 
	             && ValidMove(v->getInfo().col, v->getInfo().row, col, row) 
			     && (! InCheck(v->getInfo().col, v->getInfo().row, col, row))) {
			    // if this step can avoid capture and capture another chess as well
				string caped{""};   // for undo
                bool firstMoved{false};
                bool secMoved{false};
                if (board[col][row]){
                  caped = board[col][row]->getType();
                  secMoved= board[col][row]->getStatus();
                }
                if (board[v->getInfo().col][v->getInfo().row]){
                  firstMoved = board[v->getInfo().col][v->getInfo().row]->getStatus();
                }
                State state{false, false, 0, "", v->getInfo().col, 
					v->getInfo().row, col, row, 0, 0, 0, 0, 
					caped, firstMoved, secMoved};
				setState(state); return true;
			    } else if (IsInCheck(v->getInfo().col, v->getInfo().row)
			    && (ValidMove(v->getInfo().col, v->getInfo().row, col, row))
				&& (! InCheck(v->getInfo().col, v->getInfo().row, col, row))) {
				   // if this step can avoid capture and put opposite King in check
			 	   bool succeed{0};
				   int c1 = v->getInfo().col; int r1 = v->getInfo().row;
				   move(c1, r1, col, row);
			       if (colour == Colour::White) {
					 if (ValidMove(col, row, KingPos[0], KingPos[1])) succeed = 1;
				     } else {
					   if (ValidMove(col, row, KingPos[2], KingPos[3])) succeed = 1;
				     }
				    move(col, row, c1, r1);
				    if (succeed) {	
					string caped{""};   // undo
                        bool firstMoved{false};
                        bool secMoved{false};
                       	if (board[col][row]){
                            caped = board[col][row]->getType();
                            secMoved= board[col][row]->getStatus();
                        }
                        if (board[v->getInfo().col][v->getInfo().row]){
                            firstMoved = board[v->getInfo().col][v->getInfo().row]->getStatus();
                        }		           
					State state{false, false, 0, "", v->getInfo().col, 
						v->getInfo().row, col, row, 0, 0, 0, 0, 
						caped, firstMoved, secMoved};
					setState(state);
					return true;
				    }
		          }
		 	   }
		    }
		 }
	   }
	}
	return false;
}

// prefer to move a chess can not only avoid immediate capture 
// and next position would not be captured as well as
// capture another chess or put the opposite King in check
// if so, return true, store the information in the state
// if not, do nothing, return false
// No-throw guarantee
bool Chessboard::MoreComplex(Colour colour, bool &Moved) {
	vector<int> col;
	vector<int> row;
	int ck; int rk;
	if (colour == Colour::White) {
		ck = KingPos[2]; rk = KingPos[3];
	} else {
		ck = KingPos[0]; rk = KingPos[1];
	}
	// consider castling
	if ((! IsInCheck(ck, rk)) && (! board[ck][rk]->getStatus())) {
		if (ck + 2 <= 7) {
			if ((!board[ck+2][rk]) 
			&& (! InCheck(ck, rk, ck+2, rk)) 
			&& Castling(ck, ck + 2, rk)) { Moved = 1; return true;}
		} else if (ck - 2 >= 0) {
			if ((!board[ck-2][rk]) 
			&& (! InCheck(ck, rk, ck-2, rk)) 
			&& Castling(ck, ck - 2, rk)) { Moved = 1; return true;}
		}
	}
	for (size_t i{0}; i < 8; ++i) {col.emplace_back(i); row.emplace_back(i);}
	for (size_t i{7}; i > 0; --i) std::swap(col[i], col[rand()%i]);
	for (size_t i{0}; i < 8; ++i) {
	  for (size_t h1{7}; h1 > 0; --h1) std::swap(row[h1], row[rand()%h1]);
	  for (size_t j{0}; j < 8; ++j) {
		Chess *v = board[col[i]][row[j]];
		if ((v) && v->getInfo().colour == colour) {
	       size_t size{v->Dir.size()};
		   vector<int> d;
			 for (size_t i{0}; i < size; ++i) d.emplace_back(i);
			 for (size_t i{size - 1}; i > 0; --i) std::swap(d[i], d[rand()%i]);
			 for (size_t i{0}; i < size; ++i) {
				int row{v->getInfo().row};
				int col{v->getInfo().col};
				while (NextInDir(col, row, v->Dir[d[i]])) {
				  if (v->getType() == "Pawn" && (! v->getStatus()) 
					&& (v->Dir[d[i]] == Direction::S || v->Dir[d[i]] == Direction::N)) {
					// consider en passant or move forward two squares at the first time
					 int x = rand() % 3; //x == 1 means try to do en passant;
					if (colour == Colour::White && x == 1 && (v->getInfo().row - 2 >= 0)
					  && (IsInCheck(col, v->getInfo().row)
					  && (! InCheck(col, v->getInfo().row, col, v->getInfo().row - 2, 1)))) {
					if (EnPassent(v->getInfo().col, v->getInfo().row, v->getInfo().row - 2, false)) {
					  Moved = 1;
					  return true;
					 }
					} else if (colour == Colour::Black && x == 1 && (v->getInfo().row + 2 <= 7)
					  && (IsInCheck(col, v->getInfo().row)
					  && (! InCheck(col, v->getInfo().row, col, v->getInfo().row + 2, 1))))	{
					  if (EnPassent(v->getInfo().col, v->getInfo().row, v->getInfo().row + 2, false)) {
						Moved = 1;
						return true;
					  }
					 }
				   }
				   // avoid immediate capture and capture another chess at the
				   // same time avoid capture as well
				  if (IsInCheck(v->getInfo().col, v->getInfo().row) && (board[col][row])
					&& ValidMove(v->getInfo().col, v->getInfo().row, col, row)
					&& (! InCheck(v->getInfo().col, v->getInfo().row, col, row))
					&& (! InCheck(v->getInfo().col, v->getInfo().row, col, row, 1))){
					string caped{""};   // undo
                    bool firstMoved{false};
                    bool secMoved{false};
                    if (board[col][row]){
                        caped = board[col][row]->getType();
                        secMoved= board[col][row]->getStatus();
                    }
                    if (board[v->getInfo().col][v->getInfo().row]){
                        firstMoved = board[v->getInfo().col][v->getInfo().row]->getStatus();
                    }
					State state{false, false, 0, "", v->getInfo().col, v->getInfo().row, 
					col, row, 0, 0, 0, 0, caped, firstMoved, secMoved};
					setState(state); return true;
					}
					}
				}
			}
		}
	}
	// prefers avoid immediate capture and next position
	// would not be captured as well or capture an 
	// opposite chess and would not be captured as well
	for (int i{0}; i < 8; ++i) {
	  for (int j{0}; j < 8; ++j) {
		Chess *v = board[col[i]][row[j]];
		if ((v) && (v->getInfo().colour == colour)) {
		  int row{v->getInfo().row};
		  int col{v->getInfo().col};
		  size_t size{v->Dir.size()};
		  vector<int> d;
		  for (size_t j{0}; j < size; ++j) d.emplace_back(j);
		  for (size_t j{size - 1}; j > 0; --j) swap(d[j], d[rand()%j]);
		  for (size_t i{0}; i < size; ++i) {
			while (NextInDir(col, row, v->Dir[d[i]])) {
			  if ((IsInCheck(v->getInfo().col, v->getInfo().row)
			    && (ValidMove(v->getInfo().col, v->getInfo().row, col, row))
				&& (! InCheck(v->getInfo().col, v->getInfo().row, col, row))
				&& (! InCheck(v->getInfo().col, v->getInfo().row, col, row, 1)))
			    || ((ValidMove(v->getInfo().col, v->getInfo().row, col, row)) && (board[col][row])
			    && (! InCheck(v->getInfo().col, v->getInfo().row, col, row))
				&& (! InCheck(v->getInfo().col, v->getInfo().row, col, row, 1)))) {
			     string caped{""};   // undo
                 bool firstMoved{false};
                 bool secMoved{false};
                 if (board[col][row]){
                    caped = board[col][row]->getType();
                    secMoved= board[col][row]->getStatus();
                 }
                 if (board[v->getInfo().col][v->getInfo().row]){
                    firstMoved = board[v->getInfo().col][v->getInfo().row]->getStatus();
                 }
				 State state{false, false, 0, "", v->getInfo().col, v->getInfo().row, 
					col, row, 0, 0, 0, 0, caped, firstMoved, secMoved};
				 setState(state); return true;
				 }
				}
			}
		   }
		}
	}
	// next pos would not be captured
	for (int i{0}; i < 8; ++i) {
	  for (int j{0}; j < 8; ++j) {
	    Chess *v = board[col[i]][row[j]];
		if ((v) && (v->getInfo().colour == colour)) {
		  int row{v->getInfo().row};
		  int col{v->getInfo().col};
		  size_t size{v->Dir.size()};
		  vector<int> d;
		  for (size_t j{0}; j < size; ++j) d.emplace_back(j);
		  for (size_t j{size - 1}; j > 0; --j) swap(d[j], d[rand()%j]);
		  for (size_t i{0}; i < size; ++i) {
		  while (NextInDir(col, row, v->Dir[d[i]])) {
			 if ((ValidMove(v->getInfo().col, v->getInfo().row, col, row))
			 && (! InCheck(v->getInfo().col, v->getInfo().row, col, row))
			 && (! InCheck(v->getInfo().col, v->getInfo().row, col, row, 1))) {
			 string caped{""};
			 bool firstMoved{false};
		     bool secMoved{false};
			 if (board[col][row]) {
				caped = board[col][row]->getType();
				secMoved = board[col][row]->getStatus();
			 }
			 if (board[v->getInfo().col][v->getInfo().row]) {
				firstMoved = board[v->getInfo().col][v->getInfo().row]->getStatus();
			 }
			 State state{false, false, 0, "", v->getInfo().col, v->getInfo().row, 
				col, row, 0, 0, 0, 0, caped, firstMoved, secMoved};
			setState(state); return true;
			 }
		  }
		}
	  }
	 }
    }
	return false;
}

// check whether it is stalemate
// No-throw guarantee
bool Chessboard::IsStalemate(Colour turn) {
	if (turn == Colour::Black) {
		if ((! CanMove(Colour::Black)) && (! IsInCheck(KingPos[0], KingPos[1]))) {
			State state{false, false, 6, "", 0, 0, 0, 0, 0, 0, 0, 0, "", 0, 0};
			setState(state);
			notifyObservers();
			return true;
		}
	}
	if (turn == Colour::White) {
		if ((! CanMove(Colour::White)) && (! IsInCheck(KingPos[2], KingPos[3]))) {
			State state{false, false, 6, "", 0, 0, 0, 0, 0, 0, 0, 0, "", 0, 0};
			setState(state);
			notifyObservers();
			return true;
		}
	}
	return false;
}

// check whether black is checkmate
// No-throw guarantee
bool Chessboard::WhiteWins() {
	if (! IsInCheck(KingPos[0], KingPos[1])) return false;
	else {
		if (CanMove(Colour::Black)) return false;
	}
	return true;
}

// check whether white is checkmate
// No-throw guarantee
bool Chessboard::BlackWins() {
	if (! IsInCheck(KingPos[2], KingPos[3])) return false;
	else {
		if (CanMove(Colour::White)) return false;
	}
	return true;
}

// translate pos into (c,r)
// Strong guarantee
static void translate(int &c, int &r, string pos) {
	if (pos[0] < 'a' || pos[0] > 'h') throw pos;
	c = pos[0] - 'a';
	if (pos[1] == '1') r = 7;
	else if (pos[1] == '2') r = 6;
	else if (pos[1] == '3') r = 5;
	else if (pos[1] == '4') r = 4;
	else if (pos[1] == '5') r = 3;
	else if (pos[1] == '6') r = 2;
	else if (pos[1] == '7') r = 1;
	else if (pos[1] == '8') r = 0;
	else throw pos;
}

// Under setup mode, change the board
// Strong guarantee
void Chessboard::changeBoard(string which, int c, int r){
	if (which == "K" || which == "k") {
		board[c][r] = new King{c, r, (which == "K") ? Colour::White : Colour::Black};
		if (which == "K") { KingPos[2] = c; KingPos[3] = r;}
		else { KingPos[0] = c; KingPos[1] = r;}
	} else if (which == "Q" || which == "q") {
		board[c][r] = new Queen{c, r, (which == "Q") ? Colour::White : Colour::Black};
	} else if (which == "B" || which == "b") {
		board[c][r] = new Bishop{c, r, (which == "B") ? Colour::White : Colour::Black};
	} else if (which == "N" || which == "n") {
		board[c][r] = new Knight{c, r, (which == "N") ? Colour::White : Colour::Black};
	} else if (which == "R" || which == "r") {
		board[c][r] = new Rook{c, r, (which == "R") ? Colour::White : Colour::Black};
	} else if (which == "P" || which == "p") {
		board[c][r] = new Pawn{c, r, (which == "P") ? Colour::White : Colour::Black};
	} else throw which;
}

// check whether the setup mode is done
// No-throw guarantee
bool Chessboard::checkDone() {
	int WKing{0}; int BKing{0};
	for (auto v: board) {
		for (auto chess: v) {
			if ((chess) && (chess->getInfo().row == 0 || chess->getInfo().row == 7) 
			&& (chess->getType() == "Pawn")) { // check pawn's position
				cout << "Pawn cannot be on the first or last row of board." << endl;
				return false;
			}
			if ((chess) && (chess->getType() == "King")) { // check whether king in check
				if (IsInCheck(chess->getInfo().col, chess->getInfo().row)) {
					cout << "King cannot be in check." << endl;
					return false;
				}
				if (chess->getInfo().colour == Colour::Black) ++BKing;
				else ++WKing;
			}
		}
	}
	// check king's number
	if (BKing != 1) {
		cout << "Incorrect number of Black King." << endl;
		return false;
	}
	if (WKing != 1) {
		cout << "Incorrect number of White King." << endl;
		return false;
	}
	return true;
}

// setup the mode
// Basic guarantee
void Chessboard::Setup(Colour &turn) {
    if (board.empty()) init();
	while (true) {
		string command;
		string which;
		string pos;
		string colour;
		cin >> command;
		if (command == "+") {
			cin >> which;
			cin >> pos;
			int c; int r;
			try { translate(c, r, pos);}
			catch (string e) { 
			    cout << "Invalid command. Please try again." << endl; 
			    continue;
			}
			Chess *temp = board[c][r];
			board[c][r] = nullptr;
			try { changeBoard(which, c, r);}
			catch (string e) { 
			    cout << "Inavlid command. Please try again." << endl; 
			    board[c][r] = temp; temp = nullptr; 
			    continue;
			}
			delete temp;
			State state{false, false, 1, which, c, r, 0, 0, 0, 0, 0, 0, "", false, false};
			setState(state);
			notifyObservers();
			cout << *this;
		} else if (command == "-") {
			cin >> pos;
			int c; int r;
			try { translate(c, r, pos);}
			catch (string e) { 
			    cout << "Invalid command. Please try again." << endl; 
			    continue;
			}
			State state{false, false, -1, "", c, r, 0, 0, 0, 0, 0, 0, "", false, false};
			setState(state);
			delete board[c][r];
			board[c][r] = nullptr;
			notifyObservers();
			cout << *this;
		} else if (command == "=") {
			cin >> colour;
			if (colour != "white" && colour != "White" 
			   && colour != "black" && colour != "Black") {
				cout << "Invalid colour name. ";
				cout << "Please enter 'white' or 'White' or 'Black' or 'black'" << endl;
			}
			State state{false, false, 2, colour, 0, 0, 0, 0, 0, 0, 0, 0, "", false, false};
			setState(state);
			notifyObservers();
			if (colour == "White" || colour == "white") {
				turn = Colour::White;
			} else {
				turn = Colour::Black;
			}
		} else if (command == "done") {
			if (checkDone()) {
			  for (auto v: board) {
				for(auto chess: v) {
				  if (chess) {
					chess->setStatus(0);
					if (chess->getType() == "King") {
					  if (chess->getInfo().colour == Colour::White) { 
						 KingPos[2] = chess->getInfo().col;
						 KingPos[3] = chess->getInfo().row;
					  } else {
						 KingPos[0] = chess->getInfo().col;
						 KingPos[1] = chess->getInfo().row;
					  }
					}
				  }
				}
			  }
			  break;
			} else {
				cout << "Cannot leave setup mode." << endl;
				cout << "Please check and continue to set up the board until succeed." << endl;
				cout << "Enjoy!" << endl;
			}
		} else {
			cout << "Invalid Command. Please try again." << endl;
		}
	}
}

std::ostream &operator<<(std::ostream &out, const Chessboard &c){
	out << *(c.td);
       	return out;
}

// find the direction from (c1, r1) to (c2, r2)
// No-throw guarantee
static Direction FindDir(int c1, int r1, int c2, int r2) {
	if (c1 == c2) {
		if (r1 > r2) return Direction::N;
		else if (r1 < r2) return Direction::S;
	} else if (c1 < c2) {
		if (r1 > r2) return Direction::NE;
		else if (r1 < r2) return Direction::SE;
		else return Direction::E;
	} else {
		if (r1 > r2) return Direction::NW;
		else if (r1 < r2) return Direction::SW;
		else return Direction::W;
	}
}

// Check whether (c1,r1) can arrive (c2,r2)
// Require: Direction is valid (c1, r1) and (c2, r2) are valid
// No-throw guarantee
bool Chessboard::CanArrive(int c1, int r1, int c2, int r2) const {
	if ((((c1 + 2 == c2) || (c1 - 2 == c2)) && ((r1 + 1 == r2) || (r1 - 1 == r2)))
		|| (((c1 + 1 == c2) || (c1 - 1 == c2)) && ((r1 + 2 == r2) || (r1 - 2 == r2)))) return true;
	Direction d = FindDir(c1, r1, c2, r2);
	if (d == Direction::S) {
		if (c1 == c2 && r1 + 1 == r2) return true;
		if (IsEmpty(c1, r1 + 1)) return CanArrive(c1, ++r1, c2, r2);
	} else if (d == Direction::N) {
		if (c1 == c2 && r1 - 1 == r2) return true;
		if (IsEmpty(c1, r1 - 1)) return CanArrive(c1, --r1, c2, r2);
	} else if (d == Direction::W) {
		if (c1 - 1 == c2 && r1 == r2) return true;
		if (IsEmpty(c1 - 1, r1)) return CanArrive(--c1, r1, c2, r2);
	} else if (d == Direction::E) {
		if (c1 + 1 == c2 && r1 == r2) return true;
		if (IsEmpty(c1 + 1, r1)) return CanArrive(++c1, r1, c2, r2);
	} else if (d == Direction::NW) {
		if (c1 - 1 == c2 && r1 - 1 == r2) return true;
		if (IsEmpty(c1 - 1, r1 - 1)) return CanArrive(--c1, --r1, c2, r2);
	} else if (d == Direction::NE) {
		if (c1 + 1 == c2 && r1 - 1 == r2) return true;
		if (IsEmpty(c1 + 1, r1 - 1)) return CanArrive(++c1, --r1, c2, r2);
	} else if (d == Direction::SW) {
		if (c1 - 1 == c2 && r1 + 1 == r2) return true;
		if (IsEmpty(c1 - 1, r1 + 1)) return CanArrive(--c1, ++r1, c2, r2);
	} else if (d == Direction::SE) {
		if (c1 + 1 == c2 && r1 + 1 == r2) return true;
		if (IsEmpty(c1 + 1, r1 + 1)) return CanArrive(++c1, ++r1, c2, r2);
	} return false;
}

// Check whether (c,r) is empty
// No-throw guarantee
bool Chessboard::IsEmpty(int c, int r) const {
	if (!board[c][r]) return true;
	else return false;
}

// Check whether (c1, r1) has different colour with (c2, r2)
// and the direction is correct and the chess in (c1,r1) can arrive(c2, r2)
// No-throw guarantee
bool Chessboard::ValidMove(int c1, int r1, int c2, int r2) {
	if ((! IsEmpty(c2, r2)) && 
	(board[c1][r1]->getInfo().colour == board[c2][r2]->getInfo().colour)) return false;
	if (! board[c1][r1]->ValidDirection(c2, r2, IsEmpty(c2, r2))) { return false;}
	if (! CanArrive(c1, r1, c2, r2)) return false;
	return true;
}

// Check whether (c, r) is in check, or it can be captured in the next step
// No-throw guarantee
bool Chessboard::IsInCheck(int c, int r) {
	for (auto v: board) {
		for (auto chess: v) {
			if ((chess) && chess->getInfo().colour != board[c][r]->getInfo().colour) {
				if (ValidMove(chess->getInfo().col, chess->getInfo().row, c, r)) 
				return true;
			}
		}
	}
       	return false;
}	

// Check whether the chess in (c1, r1) moving to (c2, r2) is valid
// that is, would not put own King in check
// highlevel is true means check whether chess moving to (c2, r2)
// would be captured after this action
// No-throw guarantee
bool Chessboard::InCheck(int c1, int r1, int c2, int r2, bool highLevel) {
	bool InCheck;
	Colour colour = board[c1][r1]->getInfo().colour;
	if (IsEmpty(c2, r2)) {
		move(c1, r1, c2, r2); // move once
		if (colour == Colour::Black) {
			if (highLevel == 1) InCheck = IsInCheck(c2, r2);
			else InCheck = IsInCheck(KingPos[0], KingPos[1]);
		} else {
			if (highLevel == 1) InCheck = IsInCheck(c2, r2);
			InCheck = IsInCheck(KingPos[2], KingPos[3]);
		}
		move(c2, r2, c1, r1); // move back
	} else {
		Chess *temp = board[c2][r2];
		board[c2][r2] = nullptr;
		move(c1, r1, c2, r2);
		if (colour == Colour::Black) {
			if (highLevel == 1) InCheck = IsInCheck(c2, r2);
			else InCheck = IsInCheck(KingPos[0], KingPos[1]);
		} else {
			if (highLevel == 1) InCheck = IsInCheck(c2, r2);
			else InCheck = IsInCheck(KingPos[2], KingPos[3]);
		}
		move(c2, r2, c1, r1);
		board[c2][r2] = temp;
		temp = nullptr;
	}
	if (InCheck) return true;
	else return false;
}

// Move back / undo the last step
// No-throw guarantee
void Chessboard::MoveBack(State s){
	const int replace{3};
	const int undo{4};
	const int undoReplace{5};
	State state;
	if(s.IsCastling) {
		move(s.c2, s.r2, s.c1, s.r1);
		board[s.c1][s.r1]->setStatus(false);
		move(s.c4, s.r4, s.c3, s.r3);
		board[s.c3][s.r3]->setStatus(false);
		state = State{true, false, undo, "", s.c2, s.r2, s.c1, s.r1, 
		s.c4, s.r4, s.c3, s.r3,"",false, false};
	} else if (s.IsEnPassant) {
		Info infor{board[s.c4][s.r4]->getInfo()};
		if (infor.colour == Colour::White) {
			board[s.c1][s.r1] = new Pawn{s.c1, s.r1, Colour::Black};
		} else {
			board[s.c1][s.r1] = new Pawn{s.c1, s.r1, Colour::White};
		}
		move(s.c4, s.r4, s.c3, s.r3);
		board[s.c3][s.r3]->setStatus(s.secMoved);// c3 r3 moved or not
		state = State{false, true, undo, "", s.c2, s.r2, s.c1, s.r1, 
		s.c4, s.r4, s.c3, s.r3, "", false, s.secMoved};
	} else if (s.SetUp == replace){
		Info infor{board[s.c2][s.r2]->getInfo()};
		delete board[s.c2][s.r2];
		if (infor.colour == Colour::White) {
			board[s.c2][s.r2] = new Pawn{s.c2, s.r2, Colour::White};
		} else {
			board[s.c2][s.r2] = new Pawn{s.c2, s.r2, Colour::Black};
		}
		move(s.c2, s.r2, s.c1, s.r1);
		board[s.c1][s.r1]->setStatus(s.firstMoved);// c1 ,r1 moved or not
		if (s.CapturedChess != "") {
                        char type{s.CapturedChess[0]};
			if (infor.colour == Colour::White){
				type = type - 'A' + 'a';
			}
		        string which{type};
                        changeBoard(which, s.c2, s.r2);
                        board[s.c2][s.r2]->setStatus(s.secMoved);
		}
		state = State{false, false, undoReplace, "", s.c2, s.r2, s.c1, s.r1, 
		0, 0, 0, 0, s.CapturedChess, s.firstMoved, s.secMoved};
	} else {
		Info infor{board[s.c2][s.r2]->getInfo()};
		move(s.c2, s.r2, s.c1, s.r1);
		board[s.c1][s.r1]->setStatus(s.firstMoved);// c1, r1 moved or not
		if (s.CapturedChess != ""){
			char type{s.CapturedChess[0]};
			if (infor.colour == Colour::White){
                                type = type - 'A' + 'a';
                        }
		        string which{type};
			changeBoard(which, s.c2, s.r2);
			board[s.c2][s.r2]->setStatus(s.secMoved);
		}
		state = State{false, false, undo, s.chess, s.c2, s.r2, s.c1, s.r1, 
		0, 0, 0, 0, s.CapturedChess, s.firstMoved, s.secMoved};
	}
	setState(state);
	notifyObservers();
	cout << *this;
}

// notify graphicsdisplay to show message if the command is resign
// No-throw guarantee
void Chessboard::resign() {
	State state{false, false, 8, "", 0, 0, 0, 0, 0, 0, 0, 0, "", 0, 0};
	setState(state);
	notifyObservers();
}



