#ifndef SYM_PSEL_BHS_H
#define SYM_PSEL_BHS_H

#include "sym_engine.h"
#include <vector>
#include <set>

class SymHNode;

//Types of strategies to choose in which direction search the abstract state spaces
enum class RelaxDirStrategy {FW, BW, BIDIR, SWITCHBACK};

class BHS : public SymEngine{
  //Parameters to decide which exploration explore
  //We explore the best exploration among those that are feasible
  int maxStepTime, maxStepNodes; // maximum time and nodes to consider a step in the original state space to be feasible
  int maxStepTimeEnd, maxStepNodesEnd;
  double timeBegin, timeEnd;

 //Parameters to decide the alloted time for a step
  //alloted = max(minAlloted, estimated*multAlloted)
  int minAllotedTime, minAllotedNodes;   // min alloted time and nodes to a step
  double ratioAllotedTime, ratioAllotedNodes; // factor to multiply the estimation
  // Proportion of time in an abstract state space wrt the original state space. 
  // If ratio is 0.5 then it is better to explore original with 10 seconds than abstract with 5.1 seconds
  // It is also multiplied by maxStepTime to decide if the abstract search is feasible or not
  double ratioAbstractTime, ratioAbstractNodes;

  //Parameters to control how much to relax the search => moved to PH
  //int maxRelaxTime, maxRelaxNodes; // maximum allowed nodes to relax the search
  //int maxAfterRelaxTime, maxAfterRelaxNodes; // maximum allowed nodes to accept the abstraction after relaxing the search 
  //double ratioRelaxTime, ratioRelaxNodes; 
  //double ratioAfterRelaxTime, ratioAfterRelaxNodes;

  RelaxDirStrategy relaxDir; //Direction for relaxed explorations
  double percentageUseful; //Percentage of nodes that can potentially prune in the frontier for an heuristic to be useful


  //Functions that determine the criterion
  bool canExplore(const SymExploration & exp);
  bool isBetter(const SymExploration & exp, const SymExploration & exp2);

  SymExploration * getBestExploration(std::set<SymExploration *> & exps, bool canExploreAll);
  SymExploration * getExploration();



  double getMaxStepTime() const;
  double getMaxStepNodes() const;  
  
  inline double getAllotedTime(double estimatedTime) const{
    return max<int>(estimatedTime*ratioAllotedTime, minAllotedTime);
  }

  inline double getAllotedNodes(double estimatedNodes) const{
    return max<int>(estimatedNodes*ratioAllotedNodes, minAllotedNodes);
  }

  inline double getMaxAfterRelaxTime() const {
    return ratioAfterRelaxTime*getMaxStepTime();
  }

  inline double getMaxAfterRelaxNodes() const {
    return ratioAfterRelaxNodes*getMaxStepNodes();
  }


 public:
  BHS(const Options &opts);
  virtual ~BHS(){}

  //  static SymEngine * create_default();
  virtual void initialize();
  virtual int step();

  virtual void print_options() const;

};



#endif
