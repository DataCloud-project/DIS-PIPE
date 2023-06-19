#ifndef SYM_PSEL_H
#define SYM_PSEL_H

#include "../option_parser.h"
#include "sym_exploration.h"
#include "sym_blind_heuristic.h"
#include "sym_bdexp.h"

class SymExploration;
class SymHierarchy;

class SymPSel{
 protected: 
  //SymBlindHeuristic blindfw, blindbw;
  bool loadExploration, saveExploration;

 public:
  SymPSel(){}
  SymPSel(const Options & opts);
 ~SymPSel(){}
 virtual void init(SymHierarchy * hierarchy);
 

};

#endif
