#ifndef SYM_HEURISTIC_H
#define SYM_HEURISTIC_H

#include "../state.h"
#include "sym_solution.h"
#include <set>
#include <iostream>


class BDD;

class SymExploration;

class SymHeuristic{

 public:
  virtual ~SymHeuristic() {} 
  SymHeuristic(){}
  SymHeuristic(const SymHeuristic&) = default;
  SymHeuristic(SymHeuristic&&) = default;
  //SymHeuristic& operator=(const SymHeuristic&) & = default;
  //SymHeuristic& operator=(SymHeuristic&&) & = default;

 protected:
  vector<SymExploration *> expIAmHeuristic; //List  of all explorations I am heuristic for

 public:
  virtual void notifyToExploration(SymExploration * exploration) = 0;

  //virtual int getH (const BDD & bdd) const = 0; //Return the minimum h value in the set of states
  virtual std::set<int> getHValues() const = 0; //Possible h values

  virtual BDD getBDD(const BDD & bdd, int fVal, int hVal) const = 0; //Subset of states with h(s) == h
  virtual BDD getBDDUpTo(const BDD & bdd, int fVal, int hVal) = 0;   //Subset of states with h(s) <= h
  //virtual int getH(const State & state) = 0;   //Compute the heuristic value of the state

  virtual void extract_path(const BDD & bdd, int h, bool fw, std::vector <const Operator *> & path) const = 0;

  virtual SymSolution checkCut(const BDD & states, int g, bool fw) const = 0;

  //  virtual State selectStateFrom(BDD states) = 0;

  virtual int getHNotClosed() const = 0;
  virtual int getFNotClosed() const = 0;


  virtual BDD notClosed() const = 0;

  //Returns which exploration may improve this value
  virtual SymExploration * getExploration(){
    return nullptr; //null by default
  }

  virtual void print(std::ostream & os) const{ os << "Undefined print: " << std::endl;}

  friend std::ostream & operator<<(std::ostream &os, const SymHeuristic & h); 
};

#endif
