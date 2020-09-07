#ifndef TEXTDISPLAY_H
#define TEXTDISPLAY_H
#include <iostream>
#include <vector>
#include "observer.h"
#include "subject.h"
#include "state.h"

class TextDisplay: public Observer<State> {
  std::vector<std::vector<char>> theDisplay;
  const int size;
  int turn;
 public:
  TextDisplay(int n);
  void notify(Subject<State> &whoNotified) override; //change whoNotified's display
  ~TextDisplay();
  friend std::ostream &operator<<(std::ostream &out, const TextDisplay &td);
};

// operator<<(std::ostream &out, const TextDisplay &td) prints the TextDisplay 
//   td to show the currenrt chess board
std::ostream&operator<<(std::ostream &out, const TextDisplay &td);
#endif

