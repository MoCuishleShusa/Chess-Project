#ifndef MOVES_H
#define MOVES_H

#include "state.h"
class Node;
class Moves{
	Node * start;
	Node * end;
	int size;
	public:
	void addMove(State s);
	State UndoLastMove();
	int getSize();
	Moves();
	~Moves();
};

#endif
