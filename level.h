#ifndef _LEVEL_H
#define _LEVEL_H
#include "state.h"

class Chessboard;

class Level {
	public:
	void move(Chessboard &, Colour, bool &);
	Level();
        virtual ~Level();
	private:
	virtual void ActualMove(Chessboard &, Colour, bool &) = 0;
};

class L1: public Level{
	public:
	void ActualMove(Chessboard &, Colour, bool &) override;
	L1();
	~L1() override;
};

class L2: public Level{
	public:
	void ActualMove(Chessboard &, Colour, bool &) override;
	L2();
	~L2() override;
};

class L3: public Level{
	public:
	void ActualMove(Chessboard &, Colour, bool &) override;
	L3();
	~L3() override;
};

class L4: public Level{
	public:
	void ActualMove(Chessboard &, Colour, bool &) override;
	L4();
	~L4() override;
};
#endif

