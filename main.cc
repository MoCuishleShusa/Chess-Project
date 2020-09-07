#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include "chessboard.h"
#include "level.h"
#include "state.h"
#include "moves.h"
using namespace std;
using std::string;

void createPlayer(string first, string second, Level *&white, Level *&black) {
    if (first == "computer1") {
        white = new L1{};
      } else if (first == "computer2") {
        white = new L2{};
      } else if (first == "computer3") {
        white = new L3{};
      } else if (first == "computer4") {
        white = new L4{};
      }
    if (second == "computer1") {
        black = new L1{};
    } else if (second == "computer2") {
        black = new L2{};
    } else if (second == "computer3") {
        black = new L3{};
    } else if (second == "computer4") {
        black = new L4{};
    }
}

int main(int argc, char *argv[]){
    cin.exceptions(ios::eofbit|ios::failbit);
    string cmd;
    Chessboard c;
    Level *white = nullptr;
    Level *black = nullptr;
    float bscore{0};
    float wscore{0};
    const int T{1};
    const int F{0};
    bool IsRunning{F};
    bool boardSet{false};
    Moves *moves{nullptr};
    Colour turn{Colour::White}; //whose turn to play
    try {
        while (true) {
            cin >> cmd;
            if (cmd == "game") {
                if (IsRunning == T) {
                    cout << "Invalid Command" << endl;
                    continue;
                }
                turn = Colour::White;
                IsRunning = T;
                string first, second;
                cin >> first >> second;
                if (!(first == "human" || first == "computer1" ||
			        first == "computer2" || first == "computer3" ||
			        first == "computer4") ||
		            !(second == "human" || second == "computer1" ||
		            second == "computer2" || second == "computer3" ||
		            second == "computer4")) {
			        cout << "Invalid Player." << endl;
			        continue;
		        }
                if (white){
                    delete white;
                    white = nullptr;
                }
                if (black){
                    delete black;
                    black = nullptr;
                }
                createPlayer(first, second,white, black);
                if (!boardSet){
                    // Initialize the chessboard
                    c.init();
                }
                cout << c;
		        if (moves) {
                    delete moves;
                }
                moves = new Moves{};
            } else if (cmd == "resign") {
                if (IsRunning == F) {
                    cout << "Invalid Command. Game is not running." << endl;
                    continue;
                }
                IsRunning = F;
                if (turn == Colour::White) {
                    cout << "Black wins!" << endl;
                    ++bscore;
                } else {
                    cout << "White wins!" << endl;
                    ++wscore;
                }
                boardSet = false;
		        delete moves;
		        moves = nullptr;
		        c.resign();
            } else if (cmd == "move") {
                if (IsRunning == F) {
                    cout << "Invalid Move. Game is not running." << endl;
                    continue;
                }
		        bool move{false};
                if ((turn == Colour::White && !white) || (turn == Colour::Black && !black)) {
                    const int size{8};
                    string pos1, pos2;
		            cin >> pos1 >> pos2;
                    istringstream p1{pos1};
                    istringstream p2{pos2};
                    char pos;
                    int c1, r1, c2, r2;
                    p1 >> pos;
                    c1 = pos - 'a';
                    p1 >> r1;
                    r1 = size - r1;
                    p2 >> pos;
                    c2 = pos - 'a';
                    p2 >> r2;
                    r2 = size - r2;
                    if (0 <= c1 && c1 < size && 0 <= c2 && c2 < size && 
                        0 <= r1 && r1 < size && 0 <= r2 && r2 < size) {
			            if (c.IsEmpty(c1, r1)) {
				            cout << "Invalid Move" << endl;
				            continue;
			            }
                        try {
                            move = c.CanMove(c1, r1, c2, r2, turn);
                        } catch(string s) {
                            if (s == "Invalid replacement.") {
                                cout << "Invalid replacement." << endl;
                                continue;
                            } else if (s == "White wins!") {
                                IsRunning = F;
                                ++wscore;
                                boardSet = false;
				                delete moves;
				                moves = nullptr;
                            } else {
                                IsRunning = F;
                                ++bscore;
                                boardSet = false;
				                delete moves;
				                moves = nullptr;
                            }
                        }
                        cout << c;
			            if (move && IsRunning == T){
			            	State MyMove{c.getState()};
			            	moves->addMove(MyMove);
		             	}
                        if (move && IsRunning == T) {
                            if (turn == Colour::White){
                                turn = Colour::Black;
                            } else {
                                turn = Colour::White;
                            }
                        } else if (!move) {
                            cout << "Invalid Move" << endl;
                        }
                    } else {
                        cout << "Invalid Move. You cannot move a chess outside chessboard." << endl;
                    }
                } else if (turn == Colour::White){
                    try{
                        white->move(c, turn, move);
                    } catch (string s){
                        IsRunning = F;
                        if (s == "White wins!") {
                            ++wscore;
                        } else {++bscore;}
                        boardSet = false;
			            delete moves;
			            moves = nullptr;
                    }
		            if (move && IsRunning == T) {
		            	State MyMove{c.getState()};
		            	moves->addMove(MyMove);
	           	    }
                    cout << c;
                    turn = Colour::Black;
                } else {
                    try{
                        black->move(c, turn, move);
                    } catch(string s){
                        IsRunning = F;
                        if (s == "Black wins!") {
                            ++bscore;
                        } else {++wscore;}
                        boardSet = false;
			            delete moves;
		             	moves = nullptr;
                    }
		            if (move && IsRunning == T) {
		              	State MyMove{c.getState()};
                        moves->addMove(MyMove);
                    }
                    cout << c;
                    turn = Colour::White;
                }
                if(IsRunning == T && c.IsStalemate(turn)){
                    IsRunning = F;
                    cout << "Stalemate!" << endl;
                    wscore += 0.5;
                    bscore += 0.5;
		            delete moves;
		            moves = nullptr;
                    boardSet = false;
                }
	           } else if (cmd == "setup") {
                    if (IsRunning == T) {
                        cout << "Invalid move." << endl;
                        continue;
                    }
                    turn = Colour::White;
                    c.Setup(turn);
                    boardSet = true;
		            if (moves) {
		        	    delete moves;
	         	     }
	         	    moves = new Moves{};
            } else if (cmd == "undo"){
                if (IsRunning == F){
                    cout << "Invalid command. Game is not running." << endl;
		            continue;
                }
                if (!moves) {
                    cout << "Invalid command. You have not moved yet." << endl;
                    continue;
                }
		        const int empty{0};
		        int size{moves->getSize()};
		        if (size == empty){
                    cout << "Invalid command. You have not moved yet." << endl;
		         } else {
                    State s{moves->UndoLastMove()};
		            c.MoveBack(s);
		            if (turn == Colour::White) turn = Colour::Black;
		            else turn = Colour::White;
                 }
	        } else {
                cout << "Invalid Command" << endl;
            }
        } 
    } catch (ios::failure &) {
        delete white;
        delete black;
	    if (moves) delete moves;
        cout << "Final Score:" << endl;
        cout << "White: " << wscore << endl;
        cout << "Black: " << bscore << endl;
    }  // Any I/O failure quits
}




