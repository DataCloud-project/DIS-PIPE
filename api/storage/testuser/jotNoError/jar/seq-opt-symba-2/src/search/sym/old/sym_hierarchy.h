#ifndef SYM_HIERARCHY_H
#define SYM_HIERARCHY_H

#include "sym_bdexp.h" // To get Dir enum

class SymPH;
class SymEngine;
class SymVariables;
class SymHNode;
class SymPH;

class SymHierarchy{
  friend class SymHNode;
  SymEngine * engine;
  SymVariables * vars;
  SymHNode * root;
    


 public:
  SymHierarchy();
  ~SymHierarchy(){}
  SymHierarchy(SymHierarchy && ) = default;
  SymHierarchy(const SymHierarchy & ) = delete;
  SymHierarchy& operator=(const SymHierarchy& ) = delete;
  SymHierarchy& operator=(SymHierarchy &&) = default;

  void init (SymEngine * eng, SymPH * policy);
  
  SymBDExp * init_exploration(Dir searchDir);
  SymBDExp * load_exploration(Dir searchDir, const string & filename);

  inline SymEngine * getEngine() const{
    return engine;
  }

  inline SymHNode * getRoot() const{
    return root;
  }

  inline SymVariables * getVars() const{
    return vars;
  }

  SymHNode * abstract(SymHNode * node, std::unique_ptr<SymAbstraction> abstraction);
  SymHNode * concrete(SymHNode * node, std::unique_ptr<SymAbstraction> abstraction);

  SymBDExp * relax(SymBDExp * bdExp, Dir dir,
		   int maxStepTime,  int maxStepNodes,
		   double ratioUseful);
};

#endif
