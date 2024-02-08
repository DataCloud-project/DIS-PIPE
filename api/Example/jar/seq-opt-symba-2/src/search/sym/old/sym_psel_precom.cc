#include "sym_psel_precom.h"

#include "sym_hierarchy.h"

#include "../timer.h"
#include "../option_parser.h"
#include "../plugin.h"


SymPSelPrecompute::SymPSelPrecompute(const Options &opts) : SymPSel(opts),
							    maxStepTime(opts.get<int>("max_step_time")),
							    maxStepNodes(opts.get<int>("max_step_nodes")),
							    maxAbstractionTime(opts.get<int>("max_abstraction_time")),
							    maxAbstractionNodes(opts.get<int>("max_abstraction_nodes")) {
}


bool SymPSelPrecompute::canExplore(SymExploration * exp) {
  return exp && exp->nextStepNodes() <= maxStepNodes 
    && exp->nextStepTime() <= maxStepTime;
}

SymExploration * SymPSelPrecompute::
getAbstractExploration(SymHierarchy & /*hierarchy*/){
  SymExploration * exp = nullptr;
  return exp;
}


SymExploration * SymPSelPrecompute::
  step(SymHierarchy & hierarchy){
  int remainingTime = maxAbstractionTime - g_timer();
  int remainingNodes = maxAbstractionNodes - hierarchy.getVars()->usedNodes();
  if(remainingTime > 0 && remainingNodes > 0){
    SymExploration * exp = getAbstractExploration(hierarchy);
    
    int stepTime = min(remainingTime, maxStepTime);
    int stepNodes = min(remainingNodes, maxStepNodes);
    exp->stepImage(stepTime, stepNodes);
    return exp;
  }else{
    //Explore original state space (always fw by now)
    SymExploration * exp =  nullptr;//hierarchy.getRoot()->getFwExp();
    //    exp->stepImage(10000000, 100000000); //TODO: replace with max int
    return exp; 
  }
}


/*
SymExploration * getExploration(ExplorationNode * expNode,
				bool fw){
}
*/


// static SymPSel *_parse(OptionParser &parser) {


//     parser.add_option<int>("max_step_time", 0,
// 			 "maximum time per step");
//     parser.add_option<int>("max_step_nodes", 0,
// 			 "maximum nodes per step");
//     parser.add_option<int>("max_abstraction_time", 0,
// 			 "maximum time per abstraction");
//     parser.add_option<int>("max_abstraction_nodes", 0,
// 			 "maximum nodes per abstraction");

//     Options opts = parser.parse();

//     SymPSel *policy = 0;
//     if (!parser.dry_run()) {
//         policy = new SymPSelPrecompute(opts);
//     }

//     return policy;
// }

// static Plugin<SymPSel> _plugin("ps_prec", _parse);
