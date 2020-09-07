#include <string>
#include "graphicsdisplay.h"
#include "window.h"
#include "state.h"
#include "subject.h"
using namespace std;

static int GS{540};
static const int WhiteCell{4};
static const int BlackCell{3};
static const int WhiteChess{0};
static const int BlackChess{2};
static const int empty{0};

// constructor
GraphicsDisplay::GraphicsDisplay(const int size, const int turn): 
	xw{}, size{size}, turn{turn}{
    const int CellSize{GS / (size + 1)};
    bool IsWhite{true};
    // draw spaces without pieces
    for(int x{CellSize * 2}; x <= GS - CellSize * 4; x = x + CellSize){
        for(int y{0}; y <= GS - CellSize * 2; y = y + CellSize){
            if (IsWhite){
                xw.fillRectangle(y + CellSize, x, CellSize, CellSize, WhiteCell);
                IsWhite = false;
            } else {
                xw.fillRectangle(y + CellSize, x, CellSize, CellSize, BlackCell);
                IsWhite = true;
            }
        }
        if (IsWhite){
            IsWhite = false;
        } else {
            IsWhite = true;
        }
    }

    //draw pieces
    const int line1{0};
    const int line2{CellSize};
    const int line3{CellSize * 6};
    const int line4{CellSize * 7};
    const int line5{CellSize * 8};
    xw.fillRectangle(CellSize, line1, CellSize * size, CellSize * 2, BlackChess);
    xw.fillRectangle(CellSize, line3, CellSize * size, CellSize * 2, WhiteChess);
    xw.drawString(CellSize * 1 + CellSize / 2, line1 + CellSize / 2, "r");
    xw.drawString(CellSize * 2 + CellSize / 2, line1 + CellSize / 2, "n");
    xw.drawString(CellSize * 3 + CellSize / 2, line1 + CellSize / 2, "b");
    xw.drawString(CellSize * 4 + CellSize / 2, line1 + CellSize / 2, "q");
    xw.drawString(CellSize * 5 + CellSize / 2, line1 + CellSize / 2, "k");
    xw.drawString(CellSize * 6 + CellSize / 2, line1 + CellSize / 2, "b");
    xw.drawString(CellSize * 7 + CellSize / 2, line1 + CellSize / 2, "n");
    xw.drawString(CellSize * 8 + CellSize / 2, line1 + CellSize / 2, "r");
    for(int i{0}; i < GS - CellSize; i = i + CellSize){
        xw.drawString(i + CellSize + CellSize / 2, line2 + CellSize / 2, "p");
    }
    xw.drawString(CellSize * 1 + CellSize / 2, line4 + CellSize / 2, "R");
    xw.drawString(CellSize * 2 + CellSize / 2, line4 + CellSize / 2, "N");
    xw.drawString(CellSize * 3 + CellSize / 2, line4 + CellSize / 2, "B");
    xw.drawString(CellSize * 4 + CellSize / 2, line4 + CellSize / 2, "Q");
    xw.drawString(CellSize * 5 + CellSize / 2, line4 + CellSize / 2, "K");
    xw.drawString(CellSize * 6 + CellSize / 2, line4 + CellSize / 2, "B");
    xw.drawString(CellSize * 7 + CellSize / 2, line4 + CellSize / 2, "N");
    xw.drawString(CellSize * 8 + CellSize / 2, line4 + CellSize / 2, "R");
    for(int i{0}; i < GS - CellSize; i = i + CellSize){
        xw.drawString(i + CellSize + CellSize / 2, line3 + CellSize / 2, "P");
    }
    
    // draw the margins
    xw.fillRectangle(0, 0, CellSize, CellSize * (size + 1), 5);
    xw.fillRectangle(CellSize, line5, CellSize * size, CellSize, 5);
    
    // write at margins
    xw.drawString(CellSize * 1 + CellSize / 2, line5 + CellSize / 2, "a");
    xw.drawString(CellSize * 2 + CellSize / 2, line5 + CellSize / 2, "b");
    xw.drawString(CellSize * 3 + CellSize / 2, line5 + CellSize / 2, "c");
    xw.drawString(CellSize * 4 + CellSize / 2, line5 + CellSize / 2, "d");
    xw.drawString(CellSize * 5 + CellSize / 2, line5 + CellSize / 2, "e");
    xw.drawString(CellSize * 6 + CellSize / 2, line5 + CellSize / 2, "f");
    xw.drawString(CellSize * 7 + CellSize / 2, line5 + CellSize / 2, "g");
    xw.drawString(CellSize * 8 + CellSize / 2, line5 + CellSize / 2, "h");
    
    xw.drawString(CellSize / 2, CellSize * 0 + CellSize / 2, "8");
    xw.drawString(CellSize / 2, CellSize * 1 + CellSize / 2, "7");
    xw.drawString(CellSize / 2, CellSize * 2 + CellSize / 2, "6");
    xw.drawString(CellSize / 2, CellSize * 3 + CellSize / 2, "5");
    xw.drawString(CellSize / 2, CellSize * 4 + CellSize / 2, "4");
    xw.drawString(CellSize / 2, CellSize * 5 + CellSize / 2, "3");
    xw.drawString(CellSize / 2, CellSize * 6 + CellSize / 2, "2");
    xw.drawString(CellSize / 2, CellSize * 7 + CellSize / 2, "1");
}

//No throw guarantee
// transfer a type of chess, s, to the its representation based on its colour
static string transfer(int colour, string s){
	if (s == "King") {
		if (colour == WhiteChess){
			return "K";
		} else {
			return "k";
		}
	} else if (s == "Queen"){
	       if (colour == WhiteChess){
		       return "Q";
	       } else {
		       return "q";
	       }
	} else if (s == "Bishop"){ 
		if (colour == WhiteChess){
			return "B";
		} else {
			return "b";
		}
	} else if (s == "Rook"){
		if (colour == WhiteChess){
			return "R";
		} else {
			return "r";
		}
	} else if (s == "Knight") {
		if (colour == WhiteChess) {
			return "N";
		} else {
			return "n";
		}
	} else if (colour == WhiteChess) {
		return "P";
	} 
	return "p";
}

//No throw guarantee
//modifies graph when a change has occured to whoNotifies
void GraphicsDisplay::notify(Subject<State> &whoNotified) {
    const int CellSize{GS / (size + 1)};
    const int space{CellSize};
    State s{whoNotified.getState()};
    const int adding{1};
    const int removing{-1};
    const int TurnColor{2};
    const int undo{4};
    const int undoReplace{5};
    const int stalemate{6};
    const int win{7};
    const int sign{6};
    const int resign{8};
    
    // check space is white or not
    bool IsWhite{(s.r1 % 2 == 0 && s.c1 % 2 == 0) || 
	         (s.r1 % 2 != 0 && s.c1 % 2 != 0)};
    bool IsW{(s.r3 % 2 == 0 && s.c3 % 2 == 0) ||
	     (s.r3 % 2 != 0 && s.c3 % 2 != 0)};
    
    // modifies graph based on different command received
    int cmd{0};
    cmd = s.SetUp;
    if (cmd == adding){
        int colour;
        if ("a" < s.chess && s.chess < "z"){
            colour = BlackChess;
        } else {
            colour = WhiteChess;
        }
        xw.fillRectangle(s.c1 * CellSize + space, s.r1 * CellSize, 
			CellSize, CellSize, colour);
        xw.drawString(s.c1 * CellSize + CellSize / 2 + space, 
			s.r1 * CellSize + CellSize / 2, s.chess);
    } else if (cmd == removing){
        if (IsWhite){
            xw.fillRectangle(s.c1 * CellSize + space, 
			    s.r1 * CellSize, CellSize, CellSize, WhiteCell);
        } else {
            xw.fillRectangle(s.c1 * CellSize + space, 
			    s.r1 * CellSize, CellSize, CellSize, BlackCell);
        }
    } else if (cmd == TurnColor){
	    if (s.chess == "White" || s.chess == "white") {
		    turn = WhiteChess;
	    } else {
		    turn = BlackChess;
	    }
    } else if (cmd == stalemate){
	    xw.fillRectangle(0, 8 * CellSize, CellSize, CellSize, sign);
            xw.drawString(0, 8 * CellSize + CellSize / 2, "Stalemate!");
    } else if (cmd == resign){
	    if (turn == WhiteChess){
	            xw.fillRectangle( 0, 8 * CellSize, CellSize,
				    CellSize, BlackChess);
                    xw.drawString(0, 8 * CellSize + CellSize / 2, "Black Wins!");
             } else {
	            xw.fillRectangle( 0, 8 * CellSize, CellSize, 
				    CellSize, WhiteChess);
                    xw.drawString(0, 8 * CellSize + CellSize / 2, "White Wins!");
             }
    } else if (cmd == win) {
	    if (turn == WhiteChess){
                    xw.fillRectangle( 0, 8 * CellSize, CellSize,
				    CellSize, WhiteChess);
                    xw.drawString(0, 8 * CellSize + CellSize / 2, "White Wins!");
             } else {
                    xw.fillRectangle( 0, 8 * CellSize, CellSize, 
				    CellSize, BlackChess);
                    xw.drawString(0, 8 * CellSize + CellSize / 2, "Black Wins!");
             }
    } else if (s.IsEnPassant || s.IsCastling) {
	xw.fillRectangle( 0, 8 * CellSize, CellSize, CellSize, 5);
        string king;
        string rook;
        string pawn;
	string p; // captured pawn
	if (cmd == undo) { // change to turn before move
		if (turn == WhiteChess) {
			turn = BlackChess;
		} else {
			turn = WhiteChess;
		}
	}
        if (turn == WhiteChess) {
                king = "K";
                rook = "R";
                pawn = "p";
		p = "P";
        } else {
                king = "k";
                rook = "r";
                pawn = "P";
		p = "p";
        }
        if (s.IsCastling){//draw King and Root
            xw.fillRectangle(s.c2 * CellSize + space,
			     s.r2 * CellSize, CellSize, CellSize, turn);
            xw.drawString(s.c2 * CellSize + CellSize / 2 + space, 
			    s.r2 * CellSize + CellSize / 2, king);
            xw.fillRectangle(s.c4 * CellSize + space, 
			    s.r4 * CellSize, CellSize, CellSize, turn);
            xw.drawString(s.c4 * CellSize + CellSize / 2 + space, 
			    s.r4 * CellSize + CellSize / 2, rook);
        } else {//draw moved Pawn after captured
            int color;
            if (turn == WhiteChess) {
                color = BlackChess;
            } else{
                color = WhiteChess;
            }
            xw.fillRectangle(s.c4 * CellSize + space,
			     s.r4 * CellSize, CellSize, CellSize, color);
            xw.drawString(s.c4 * CellSize + CellSize / 2 + space, 
			     s.r4 * CellSize + CellSize / 2, pawn);
        }
	if (s.IsEnPassant && cmd == undo){// draw captured Pawn
	    xw.fillRectangle(s.c2 * CellSize + space,
			     s.r2 * CellSize, CellSize, CellSize, turn);
            xw.drawString(s.c2 * CellSize + CellSize / 2 + space,
			    s.r2 * CellSize + CellSize / 2, p);
	}
        if (IsWhite) {// draw empty space after King moved
            xw.fillRectangle(s.c1 * CellSize + space, 
			    s.r1 * CellSize, CellSize, CellSize, WhiteCell);
        } else {
            xw.fillRectangle(s.c1 * CellSize + space, 
			    s.r1 * CellSize, CellSize, CellSize, BlackCell);
        }
        if (IsW){// draw empty space after Root moved
            xw.fillRectangle(s.c3 * CellSize + space, 
			    s.r3 * CellSize, CellSize, CellSize, WhiteCell);
        } else {
            xw.fillRectangle(s.c3 * CellSize + space,
			    s.r3 * CellSize, CellSize, CellSize, BlackCell);
        }
	if (cmd != undo){
            if (turn == WhiteChess){ 
		turn = BlackChess;
	    } else {
		turn = WhiteChess;
	    }
	} 
    } else if (cmd == undoReplace) {
	string p;
	int turnAfter{turn};
	if (turn == WhiteChess) {
            turn = BlackChess;
            p = "p";
        } else {
            turn = WhiteChess;
            p = "P";
        }

	//draw replaced Pawn
	xw.fillRectangle(s.c2 * CellSize + space, 
			s.r2 * CellSize, CellSize, CellSize, turn);
	xw.drawString(s.c2 * CellSize + CellSize / 2 + space,
		       	s.r2 * CellSize + CellSize / 2, p);
	if (s.CapturedChess == ""){// with no piece been captured
	    if (IsWhite) {
		xw.fillRectangle(s.c1 * CellSize + space,
			       	s.r1 * CellSize, CellSize, CellSize, WhiteCell);
            } else {
                xw.fillRectangle(s.c1 * CellSize + space,
			       	s.r1 * CellSize, CellSize, CellSize, BlackCell);
            }
	} else {
	    string caped{transfer(turnAfter, s.CapturedChess)};
	    xw.fillRectangle(s.c1 * CellSize + space, 
			    s.r1 * CellSize, CellSize, CellSize, turnAfter);
            xw.drawString(s.c1 * CellSize + CellSize / 2 + space,
			    s.r1 * CellSize + CellSize / 2, caped);
	}
    } else {// common move or capture
	xw.fillRectangle( 0, 8 * CellSize, CellSize, CellSize, 5);
	int turnAfter = turn;
	if (cmd == undo){
	    if (turn == WhiteChess) {
                turn = BlackChess;
            } else {
                turn = WhiteChess;
            }
	}
        xw.fillRectangle(s.c2 * CellSize + space, s.r2 * CellSize, 
			CellSize, CellSize, turn);
	string type = {transfer(turn, s.chess)};
        xw.drawString(s.c2 * CellSize + CellSize / 2 + space,
		       	s.r2 * CellSize + CellSize / 2, type);
	if (s.CapturedChess != "" && cmd == undo) {
            string caped{transfer(turnAfter, s.CapturedChess)};
            xw.fillRectangle(s.c1 * CellSize + space, 
			    s.r1 * CellSize, CellSize, CellSize, turnAfter);
            xw.drawString(s.c1 * CellSize + CellSize / 2 + space, 
			    s.r1 * CellSize + CellSize / 2, caped);
	} else {
	    if (IsWhite) {
                xw.fillRectangle(s.c1 * CellSize + space, 
				s.r1 * CellSize, CellSize, CellSize, WhiteCell);
            } else {
                xw.fillRectangle(s.c1 * CellSize + space,
			       	s.r1 * CellSize, CellSize, CellSize, BlackCell);
            }
	}
	if (cmd != undo){
	    if (turn == WhiteChess) {
		turn = BlackChess;
	    } else {
		turn = WhiteChess;
	    }
	}
    }

}
 
// destructor
GraphicsDisplay::~GraphicsDisplay(){}




