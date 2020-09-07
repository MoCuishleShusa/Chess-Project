#include <vector>
#include <string>
#include "chess.h"
#include "info.h"
#include "state.h"
using namespace std;
using std::string;

// No throw guarantee
void Chess::setRow(int r){
	row = r;
}

// No throw guarantee
void Chess::setCol(int c){
	col = c;
}

// No throw guarantee
Info Chess::getInfo() const{ //get infomation of col, row, colour
	Info info{col, row, colour};
	return info;
}

// No throw guarantee
string Chess::getType() const{ //get exact chess type
	return this->getChessType();
}


bool Chess::getStatus() const{
	return IsMoved;
}

// No throw guarantee
void Chess::setStatus(bool s){// once moved, turn IsMoved to be true
	IsMoved = s;
}

// No throw guarantee
// check whether going to (c, r) is valid
bool Chess::ValidDirection(int c, int r, bool IsEmpty) const{
	return this->IsValidDir(c, r, IsEmpty);
}

//constructor
Chess::Chess(int col, int row, Colour colour):
    col{col}, row{row}, colour{colour}, IsMoved{false}, Dir{}{}

//destructor
Chess::~Chess(){
	int s{0};
        s = Dir.size();
	while(s){
		Dir.pop_back();
		--s;
	}
}

// No throw guarantee
// check if it is valid to move this Chess King to (c, r)
bool King::IsValidDir(int c, int r, bool IsEmpty) const {
	Info infor{getInfo()};
	const int row{infor.row};
	const int col{infor.col};
	const int limitedStep{1};
	const int castling{2};
	if (r == row && abs(c - col) == castling){
		return !getStatus() && IsEmpty;
	}
	if(row == r && abs(c - col) == limitedStep) return true;
	if(col == c && abs(r - row) == limitedStep) return true;
	if(abs(r - row) == limitedStep && abs(c - col) == limitedStep) return true;
	return false;
}

string King::getChessType() const {
	return "King";
}

// constructor
King::King(int col, int row, Colour colour): Chess{col, row, colour} {
	Dir.emplace_back(Direction::NW);
	Dir.emplace_back(Direction::N);
	Dir.emplace_back(Direction::NE);
	Dir.emplace_back(Direction::W);
	Dir.emplace_back(Direction::E);
	Dir.emplace_back(Direction::SW);
	Dir.emplace_back(Direction::S);
	Dir.emplace_back(Direction::SE);
}

//destructor
King::~King(){ }

// No throw guarantee
// check if it is valid to move this Chess Queen to (c, r)
bool Queen::IsValidDir(int c, int r, bool IsEmpty) const {
	Info infor{getInfo()};
	const int row{infor.row};
	const int col{infor.col};
	if (c != col && r == row) return true;
	if (r != row && c == col) return true;
	if (abs(r - row) == abs(c - col) && row != r && col != c) return true;
	return false;
}

string Queen::getChessType() const {
	return "Queen";
}

// constructor
Queen::Queen(int col, int row, Colour colour): Chess{col, row, colour} {
    Dir.emplace_back(Direction::NW);
	Dir.emplace_back(Direction::N);
	Dir.emplace_back(Direction::NE);
	Dir.emplace_back(Direction::W);
	Dir.emplace_back(Direction::E);
	Dir.emplace_back(Direction::SW);
	Dir.emplace_back(Direction::S);
	Dir.emplace_back(Direction::SE);
}

// destructor
Queen::~Queen(){}

// No throw guarantee
// check if it is valid to move this Chess Bishop to (c, r)
bool Bishop::IsValidDir(int c, int r, bool IsEmpty) const {
	Info infor{getInfo()};
	const int row{infor.row};
	const int col{infor.col};
	if (abs(r - row) == abs (c - col) && row != r && col != c){
		return true;
	}
	return false;
}

string Bishop::getChessType() const {
	return "Bishop";
}

// constructor
Bishop::Bishop(int col, int row, Colour colour): Chess{col, row, colour} {
    Dir.emplace_back(Direction::NW);
	Dir.emplace_back(Direction::NE);
	Dir.emplace_back(Direction::SW);
	Dir.emplace_back(Direction::SE);
}

Bishop::~Bishop(){}

// No throw guarantee
// check if it is valid to move this Chess Rook to (c, r)
bool Rook::IsValidDir(int c, int r, bool IsEmpty) const {
	Info infor{getInfo()};
	const int row{infor.row};
	const int col{infor.col};
	if (row == r && col != c) return true;
	if (col == c && row != r) return true;
	return false;
}

string Rook::getChessType() const {
	return "Rook";
}

// constructor
Rook::Rook(int col, int row, Colour colour): Chess{col, row, colour}{
	Dir.emplace_back(Direction::N);
	Dir.emplace_back(Direction::W);
	Dir.emplace_back(Direction::E);
	Dir.emplace_back(Direction::S);
}

// destructor
Rook::~Rook(){}

// No throw guarantee
// check if it is valid to move this Chess Knight to (c, r)
bool Knight::IsValidDir(int c, int r, bool IsEmpty) const {
	Info infor{getInfo()};
	const int row{infor.row};
	const int col{infor.col};
	const int maxStep{2};
	const int minStep{1};
	if (abs(r - row) == maxStep && abs(c - col) == minStep) return true;
	if (abs(c - col) == maxStep && abs(r - row) == minStep) return true;
	return false;
}

string Knight::getChessType() const {
	return "Knight";
}

// constructor
Knight::Knight(int col, int row, Colour colour):Chess{col, row, colour}{
    Dir.emplace_back(Direction::K1);
	Dir.emplace_back(Direction::K2);
	Dir.emplace_back(Direction::K3);
	Dir.emplace_back(Direction::K4);
	Dir.emplace_back(Direction::K5);
	Dir.emplace_back(Direction::K6);
	Dir.emplace_back(Direction::K7);
	Dir.emplace_back(Direction::K8);
}

// destructor
Knight::~Knight(){}

// No throw guarantee
// check if it is valid to move this Chess Pawn to (c, r) with information
//  that whether (c, r) is empty or not
bool Pawn::IsValidDir(int c, int r, bool IsEmpty) const {
	Info infor{getInfo()};
	const int row{infor.row};
	const int col{infor.col};
	const int limitedStep{1};
	const int maxStep{2};
	Colour color{infor.colour};
	if (color == Colour::Black){
		if(r - row == limitedStep && abs(c - col) == limitedStep){
			 return !IsEmpty;
		 }
		if(col == c && r - row == maxStep) return !getStatus() && IsEmpty;
		if(col == c && r - row == limitedStep) return IsEmpty;
	}
	if (color == Colour::White){
		if(row - r == limitedStep && abs(c - col) == limitedStep){
			return !IsEmpty;
		}
		if(col == c && row - r == maxStep) return !getStatus() && IsEmpty;
		if(col == c && row - r == limitedStep) return IsEmpty;
	}
	return false;
}

string Pawn::getChessType() const {
	return "Pawn";
}

// constructor
Pawn::Pawn(int col, int row, Colour colour): Chess{col, row, colour}{
	if (colour == Colour::Black){
		Dir.emplace_back(Direction::S);
		Dir.emplace_back(Direction::SW);
		Dir.emplace_back(Direction::SE);
	} else{
		Dir.emplace_back(Direction::N);
		Dir.emplace_back(Direction::NW);
		Dir.emplace_back(Direction::NE);
	}
}

// destructor
Pawn::~Pawn(){}

