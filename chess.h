#ifndef _CHESS_H
#define _CHESS_H
#include <vector>
#include <string>
#include "state.h"
#include "info.h"

class Chess {
	int col, row;
	Colour colour;
	bool IsMoved;
	public: 
	std::vector<Direction> Dir;

	public:
	void setRow(int r); //row in board
	void setCol(int c); //col in board
	Info getInfo() const; //get row, col, colour of chess in board 
	std::string getType() const; //get exact chess type
	bool getStatus() const; //get IsMoved
	void setStatus(bool); //Once moved, turn IsMoved to be true
	bool ValidDirection(int c, int r, bool IsEmpty) const; //check whether going to (c, r) is valid
	Chess(int col, int row, Colour);
	virtual ~Chess();

	private:
	virtual bool IsValidDir(int c, int r, bool IsEmpty) const = 0;
	virtual std::string getChessType() const = 0;
};

class King: public Chess{
	public:
	bool IsValidDir(int c, int r, bool IsEmpty) const override;
	std::string getChessType() const override;
	King(int col, int row, Colour);
	~King() override;
};

class Queen: public Chess{
	public:
	bool IsValidDir(int c, int r, bool IsEmpty) const override;
	std::string getChessType() const override;
	Queen(int col, int row, Colour);
	~Queen() override;
};

class Bishop: public Chess{
	public:
	bool IsValidDir(int c, int r, bool IsEmpty) const override;
	std::string getChessType() const override;
	Bishop(int col, int row, Colour);
	~Bishop() override;
};

class Rook: public Chess{
	public:
	bool IsValidDir(int c, int r, bool IsEmpty) const override;
	std::string getChessType() const override;
	Rook(int col, int row, Colour);
	~Rook() override;
};

class Knight: public Chess{
	public:
	bool IsValidDir(int c, int r, bool IsEmpty) const override;
	std::string getChessType() const override;
	Knight(int col, int row, Colour);
	~Knight() override;
};

class Pawn: public Chess{
	public:
	bool IsValidDir(int c, int r, bool IsEmpty) const override;
	std::string getChessType() const override;
	Pawn(int col, int row, Colour);
	~Pawn() override;
};
#endif



