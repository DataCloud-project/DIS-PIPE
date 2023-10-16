#ifndef SYM_BLIND_HEURISTIC_H
#define SYM_BLIND_HEURISTIC_H

#include "sym_heuristic.h"
#include "../state.h"
#include <vector>
#include "sym_variables.h"
#include "sym_manager.h"

class SymBlindHeuristic : public SymHeuristic{


  int hNotGoal;
  BDD goal;

 public:
  SymBlindHeuristic();

  ~SymBlindHeuristic(){}
  SymBlindHeuristic(const SymBlindHeuristic&) = default;
  SymBlindHeuristic(SymBlindHeuristic&&) = default;
  //  SymBlindHeuristic& operator=(const SymBlindHeuristic &) & = default;
  //  SymBlindHeuristic& operator=(SymBlindHeuristic&&) & = default;


  void init(SymManager * manager, bool fw);
  /*  SymBlindHeuristic (const State & state);
      SymBlindHeuristic (std::vector<std::pair<int, int> > & state);*/

  //Return the minimum h value in the set of states
  virtual int getH (const BDD & bdd) const; 
  
  //Possible h values
  virtual std::set<int> getHValues() const; 
  
  //Subset of states with h(s) == h
  virtual BDD getBDD(const BDD & bdd, int h) const;

  //Subset of states with h(s) <= h
  virtual BDD getBDDUpTo(const BDD & bdd, int h);

  //Compute the heuristic value of the state
  //virtual int getH(const State & state) ;

  virtual void extract_path(const BDD & bdd,  int h, bool fw,
			    std::vector <const Operator *> & path) const;

  virtual SymSolution checkCut(const BDD & states, int g, bool fw) const;

  /*  virtual State selectStateFrom(BDD states){
    return *g_initial_state;
  }*/

  int getHNotClosed() const{
    return hNotGoal;
  }

  int getFNotClosed() const{
    return 0;
  }

  virtual BDD notClosed() const {
    return !goal;
  }

  void notifyToExploration(SymExploration * /*exploration*/){}

};
#endif
