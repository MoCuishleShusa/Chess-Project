#include "node.h"
#include "state.h"

// No throw guarantee
void Node::addNext(Node * node){
	next = node;
	node->prev = this;
}

// No throw guarantee
State Node::UndoMove(Node *&node){
	if (prev) {
		prev->next = nullptr;
	}
	node = prev;
	prev = nullptr;
	return s;
}

// constructor
Node::Node(State s):s{s}, prev{nullptr}, next{nullptr}{}

// destructor
Node::~Node(){
	delete next;
}
