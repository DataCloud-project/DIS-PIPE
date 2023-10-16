#include "sym_blind_heuristic.h"

using namespace std;
SymBlindHeuristic::SymBlindHeuristic() : hNotGoal(0){}

void SymBlindHeuristic::init (SymManager * mgr, bool fw) {
  hNotGoal = mgr->hasTransitions0() ? 0 : 1;
  if(fw)
    goal = mgr->getGoal();
  else
    goal = mgr->getInitialState();
}  


//Return the minimum h value in the set of states
int SymBlindHeuristic::getH (const BDD & bdd) const{
  BDD c = bdd*goal;
  if(c.IsZero()){
    return hNotGoal;
  }else{
    return 0;
  }
}
  
//Possible h values
set<int> SymBlindHeuristic::getHValues() const{
  set<int> res;
  res.insert(hNotGoal);
  return res;
}
  
//Subset of states with h(s) == h
BDD SymBlindHeuristic::getBDD(const BDD & bdd, int /*h*/) const{
  return bdd;
}

//Subset of states with h(s) <= h
BDD SymBlindHeuristic::getBDDUpTo(const BDD & bdd, int /*h*/){
  return bdd;
}

void SymBlindHeuristic::extract_path(const BDD & /*bdd*/,int /*h*/, bool /*fw*/, vector <const Operator *> & /*path*/) const{
   
}

SymSolution SymBlindHeuristic::checkCut(const BDD & states, int /*g*/, bool /*fw*/) const {
  BDD cut = states*goal;
  if(cut.IsZero()){
    return SymSolution(); //No solution yet :(
  }
  /*TODO:
  if(fw)
    return SymSolution(g, 0, cut);
  else
    return SymSolution(0, g, cut);*/
  return SymSolution();
}
