#include "state.h"
#include "moves.h"
#include "node.h"
using namespace std;

// constructor
Moves::Moves(): start{nullptr}, end{nullptr}, size{0}{}

// destructor
Moves::~Moves(){
        delete start;
}

// No throw guarantee
// add move just occur 
void Moves::addMove(State s){
        Node *node{new Node{s}};
        if (start){
                end->addNext(node);
                end = node;
        } else {
                start = node;
                end = node;
        }
        ++size;
}


// No throw guarantee
// remove the latest move occured and return that move
// requires: size > 0
State Moves::UndoLastMove(){
        Node *newEnd{nullptr};
        State s{end->UndoMove(newEnd)};
        delete end;
        end = newEnd;
        --size;
	if (size == 0){
		start = nullptr;
		end = nullptr;
	}
        return s;
}

// return the number of moves occured
int Moves::getSize(){
        return size;
}
