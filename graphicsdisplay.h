#ifndef _GRAPHICS_H_
#define _GRAPHICS_H_
#include "window.h"
#include "observer.h"
#include "subject.h"
#include "state.h"

class GraphicsDisplay: public Observer<State> {
	Xwindow xw;
	const int size; //How many cells each row or each column has
	int turn; // who's turn 
	public:
	//Constructor
	GraphicsDisplay(const int size, const int turn=0);// 0 stands for white
	void notify(Subject<State> &whoNotified) override;
	~GraphicsDisplay();
};
#endif


