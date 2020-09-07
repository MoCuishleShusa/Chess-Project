#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include "subject.h"
#include "state.h"
#include "textdisplay.h"
using namespace std;

static const int white{0};
static const int black{1};

// constructor 
TextDisplay::TextDisplay(int n):theDisplay{}, size{n}, turn{white}{
    bool IsWhite{true};
    for(int i{0}; i < size; ++i){// adding chess spaces
        std::vector<char> line;
        for (int j{0}; j < size; ++j){
            if (IsWhite) {
                line.emplace_back(' ');
                IsWhite = false;
            } else {
                line.emplace_back('-');
                IsWhite = true;
            }
        }
        if (IsWhite) {
            IsWhite = false;
        } else {
            IsWhite = true;
        }
        theDisplay.emplace_back(line);
    }
    theDisplay[0][0] = 'r'; // putting chess pieces
    theDisplay[0][1] = 'n';
    theDisplay[0][2] = 'b';
    theDisplay[0][3] = 'q';
    theDisplay[0][4] = 'k';
    theDisplay[0][5] = 'b';
    theDisplay[0][6] = 'n';
    theDisplay[0][7] = 'r';
    
    theDisplay[1][0] = 'p';
    theDisplay[1][1] = 'p';
    theDisplay[1][2] = 'p';
    theDisplay[1][3] = 'p';
    theDisplay[1][4] = 'p';
    theDisplay[1][5] = 'p';
    theDisplay[1][6] = 'p';
    theDisplay[1][7] = 'p';
    
    theDisplay[6][0] = 'P';
    theDisplay[6][1] = 'P';
    theDisplay[6][2] = 'P';
    theDisplay[6][3] = 'P';
    theDisplay[6][4] = 'P';
    theDisplay[6][5] = 'P';
    theDisplay[6][6] = 'P';
    theDisplay[6][7] = 'P';
    
    theDisplay[7][0] = 'R';
    theDisplay[7][1] = 'N';
    theDisplay[7][2] = 'B';
    theDisplay[7][3] = 'Q';
    theDisplay[7][4] = 'K';
    theDisplay[7][5] = 'B';
    theDisplay[7][6] = 'N';
    theDisplay[7][7] = 'R';
}

// transfer changes a type of chess to corresponding string representation
static string transfer(int colour, string s){
	if (s == "King") {
		if (colour == white){
			return "K";
		} else {
			return "k";
		}
	} else if (s == "Queen"){
	       if (colour == white){
		       return "Q";
	       } else {
		       return "q";
	       }
	} else if (s == "Bishop"){ 
		if (colour == white){
			return "B";
		} else {
			return "b";
		}
	} else if (s == "Rook"){
		if (colour == white){
			return "R";
		} else {
			return "r";
		}
	} else if (s == "Knight") {
		if (colour == white) {
			return "N";
		} else {
			return "n";
		}
	} else if (colour == white) {
		return "P";
	} 
	return "p";
}

// No throw guarantee
// notify textdisplay when a change has occurred in whoNotified
void TextDisplay::notify(Subject<State> &whoNotified) {
    State s{whoNotified.getState()};
    const int adding{1};
    const int removing{-1};
    const int turning{2};
    const int replace{3};
    const int undo{4};
    const int undoReplace{5};
    const int stalemate{6};
    const int win{7};
    const int resign{8};

    // check the space is white or not
    bool IsWhite{(s.r1 % 2 == 0 && s.c1 % 2 == 0) ||
	         (s.r1 % 2 != 0 && s.c1 % 2 != 0)};
    bool IsW{(s.r3 % 2 == 0 && s.c3 % 2 == 0) || 
	     (s.r3 % 2 != 0 && s.c3 % 2 != 0)};

    // modify textdisplay
    if (s.SetUp == adding){
        theDisplay[s.r1][s.c1] = s.chess[0];
    } else if (s.SetUp == removing){
        if (IsWhite){
            theDisplay[s.r1][s.c1] = ' ';
        } else {
            theDisplay[s.r1][s.c1] = '-';
        }
    } else if (s.SetUp == turning) {
	if (s.chess == "White" || s.chess == "white") {	
	    turn = white;
         } else {
	    turn = black;
         }
    } else if (s.SetUp == stalemate || s.SetUp == win || s.SetUp == resign) {
	    return;
    } else if (s.IsEnPassant || s.IsCastling) {
        if (s.IsCastling){
            theDisplay[s.r2][s.c2] = theDisplay[s.r1][s.c1];
        }
        if (IsWhite) {
            theDisplay[s.r1][s.c1] = ' ';
        } else {
            theDisplay[s.r1][s.c1] = '-';
        }
        theDisplay[s.r4][s.c4] = theDisplay[s.r3][s.c3];
	if (s.SetUp == undo && s.IsEnPassant) {
            if(theDisplay[s.r4][s.c4] == 'P'){
	       theDisplay[s.r2][s.c2] = 'p';
	    } else { 
	       theDisplay[s.r2][s.c2] = 'P';
	    } 
	}
        if (IsW){
            theDisplay[s.r3][s.c3] = ' ';
        } else {
            theDisplay[s.r3][s.c3] = '-';
        }
	if (turn == white){
	    turn = black;
	} else {
	    turn = white;
	}
    } else if (s.SetUp == undoReplace){
	if ('a' <= theDisplay[s.r1][s.c1] && theDisplay[s.r1][s.c1] <= 'z'){
            theDisplay[s.r2][s.c2] = 'p';
        } else {
            theDisplay[s.r2][s.c2] = 'P';
        } 
        if (s.CapturedChess == ""){
            if (IsWhite){
                theDisplay[s.r1][s.c1] = ' ';
            } else{
	        theDisplay[s.r1][s.c1] = '-';
	    }
        } else {
	    string caped{transfer(turn, s.CapturedChess)};
            theDisplay[s.r1][s.c1] = caped[0];
        }
	if (turn == white){
            turn = black;
        } else {
            turn = white;
        }
    } else {
	if (s.SetUp == replace) {
	    string str{transfer(turn, s.chess)};
            theDisplay[s.r2][s.c2] = str[0];
	} else {
       	    theDisplay[s.r2][s.c2] = theDisplay[s.r1][s.c1];
	}
	if (s.SetUp == undo && s.CapturedChess != ""){
	    string caped{transfer(turn, s.CapturedChess)};
            theDisplay[s.r1][s.c1] = caped[0];
	} else {
            if (IsWhite){
                theDisplay[s.r1][s.c1] = ' ';
             } else{
                theDisplay[s.r1][s.c1] = '-';
             }
	}
	if (turn == white){
		turn = black;
	} else {
		turn = white;
	}
    }
}


// destructor
TextDisplay::~TextDisplay(){
    for(int i{size - 1}; i >= 0; --i){
        for(int j{0}; j < size; ++j){
            theDisplay[i].pop_back();
        }
        theDisplay.pop_back();
    }
}


// print out Textdisplay td
std::ostream &operator<<(std::ostream &out, const TextDisplay &td) {
    for(int i{0}; i < td.size; ++i){
        out << td.size - i << " ";
        for(int j{0}; j < td.size; ++j){
            out << td.theDisplay[i][j];
        }
        out << std::endl;
    }
    out << "  "  << "abcdefgh" << std::endl;
    return out;
}


