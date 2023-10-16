
#ifndef SYM_ADD_HEURISTIC_H
#define SYM_ADD_HEURISTIC_H

class SymADDHeuristic : public Heuristic {
 protected:
  
  vector <ADD> adds;
  
  int hNotClosed;
 public: 
  SymADDHeuristic(const Options &options);
  virtual ~SymADDHeuristic(){}

  virtual int compute_heuristic(const State &state);
  virtual void initialize () = 0;
};

#endif
