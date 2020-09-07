#ifndef SUBJECT_H
#define SUBJECT_H
#include <vector>

/* A note on StateType:

   StateType records the start position and end position 
   currently considered to be moved or just have been moved
*/


template <typename StateType> class Observer;

template <typename StateType> class Subject {
  std::vector<Observer<StateType>*> observers;
  StateType state;
 protected:
  void setState(StateType newS);
 public:
  void attach(Observer<StateType> *o);
  void detach(Observer<StateType> *o);
  void notifyObservers();
  StateType getState() const;
};

template <typename StateType>
void Subject<StateType>::detach(Observer<StateType> *o){
    for(auto it = observers.begin(); it != observers.end(); ++it){
        if (*it == o){
            observers.erase(it);
            delete o;
            break;
        }
    }
}

template <typename StateType>
void Subject<StateType>::attach(Observer<StateType> *o) {
  observers.emplace_back(o);
}

template <typename StateType>
void Subject<StateType>::notifyObservers() {
  for (auto &ob : observers){
    ob->notify(*this);
  }
}

template <typename StateType>
void Subject<StateType>::setState(StateType newS) {
  state = newS; 
}

template <typename StateType>
StateType Subject<StateType>::getState() const {
  return state;
}
#endif

