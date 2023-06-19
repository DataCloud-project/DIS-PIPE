
#include "sym_add_heuristic"

SymADDHeuristic::SymADDHeuristic(const Options &options) : Heuristic (options), 
							   hNotClosed(0) {}


int SymADDHeuristic::compute_heuristic(const State &state){
  for (const ADD heuristicADD & : adds){
    int * inputs = mgr->getBinaryDescription(state);
    ADD evalNode = heuristicADD.Eval(inputs);
    int h_value = Cudd_V(evalNode.getRegularNode());
    if(h_value >= 0){
      return h_value;
    }
  }
  if(hNotClosed < 0){
    return DEAD_END;
  }else{
    return hNotClosed;
  }

}
