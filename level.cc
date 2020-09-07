#include <iostream>
#include "level.h"
#include "chessboard.h"
#include "subject.h"
using namespace std;


// constructor and destructor
Level::Level(){}
Level::~Level(){}

L1::L1(): Level(){}
L1::~L1(){}

L2::L2(): Level{}{}
L2::~L2(){}

L3::L3(): Level{}{}
L3::~L3(){}

L4::L4(): Level{}{}
L4::~L4(){}

// strong guarantee
// computer moves
void Level::move(Chessboard &c, Colour colour, bool &move) {
	ActualMove(c, colour, move);
}

// strong guarantee
// computer1 moves
void L1::ActualMove(Chessboard &c, Colour colour, bool &move){
    if (c.CanMove(colour)){
	move = true;
        State s{c.getState()};
        c.ActualMove(s.c1, s.r1, s.c2, s.r2, colour, false);
    }
}

// strong guarantee
// computer2 moves
void L2::ActualMove(Chessboard &c, Colour colour, bool &move){
	cout << "hereh" << endl;
    if(c.CanCapture(colour) || c.CanMove(colour)){
        move = true;
	State s{c.getState()};
        c.ActualMove(s.c1, s.r1, s.c2, s.r2, colour, false);
    }
}

// strong guarantee
// computer3 moves
void L3::ActualMove(Chessboard &c, Colour colour, bool &move){
    if (c.CanAvoidCapture(colour) || c.CanCapture(colour) || 
		    c.CanMove(colour)){
	move = true;
        State s{c.getState()};
        c.ActualMove(s.c1, s.r1, s.c2, s.r2, colour, false);
    }
}

// strong guarantee
// computer4 moves
void L4::ActualMove(Chessboard &c, Colour colour, bool &move) {
    bool moved{false};
    if (c.MoreComplex(colour, moved) || c.CanAvoidCapture(colour) ||
        c.CanCapture(colour) || c.CanMove(colour)){
	move = true;
	if (moved) return;
	State s{c.getState()};
        c.ActualMove(s.c1, s.r1, s.c2, s.r2, colour, false);
    }  
}



