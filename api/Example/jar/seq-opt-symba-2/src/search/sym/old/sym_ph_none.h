#ifndef SYM_PH_NONE_H
#define SYM_PH_NONE_H

#include "sym_ph.h"


//Default abstraction: it only has the original state space
class SymPHNone : public SymPH{
 public:
  SymPHNone(const Options &opts);
  virtual ~SymPHNone(){}

  virtual std::unique_ptr<SymAbstraction> abstract(SymAbstraction * other);
  //virtual std::unique_ptr<SymAbstraction> concrete(SymAbstraction * other);

  virtual std::unique_ptr<SymAbstraction> noneAbstraction();

  virtual void getListAbstraction(SymHNode * hNode, vector<SymHNode *> & res);
  
  virtual int numPossibleAbstractions(SymAbstraction * abstraction);
  static SymPH * create_default();
};

#endif
