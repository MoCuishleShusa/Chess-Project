#ifndef NODE_H
#define NODE_H

#include "state.h"
class Node{
       	State s;
        Node * prev;
        Node * next;
        public:
        void addNext(Node * node);
        State UndoMove(Node * &node);
        Node(State s);
        ~Node();
};

#endif
