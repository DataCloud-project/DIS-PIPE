#ifndef SYM_PSEL_PRECOM_H
#define SYM_PSEL_PRECOM_H

#include "sym_psel.h"

class SymPSelPrecompute : public SymPSel{
  
  //Attributes to limit the time and memory used by the abstractions/each abstraction/each step
  int maxStepTime, maxStepNodes;
  int maxAbstractionTime, maxAbstractionNodes;
  //int maxExplorationTime, maxExplorationNodes;

 protected:
  //Functions that determine the criterion
  bool canExplore(SymExploration * exp);

  SymExploration * getAbstractExploration(SymHierarchy & hierarchy);
 public:

  SymPSelPrecompute(const Options &opts);

  virtual SymExploration * step(SymHierarchy & hierarchy);
};

#endif
