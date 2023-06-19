#ifndef SYM_DIJKSTRA_H
#define SYM_DIJKSTRA_H

#include "sym_exploration.h"
#include <vector> 

/*
 * The open list stores sets of BDDs instead a single BDD defining the state set.
 * This has the advantage of not doing disjunctions if it is not neccessary and 
 * applying shrinking before doing disjunctions.
 */
class SymDijkstra : public SymExploration {
 private:
  std::map <int, std::vector <BDD>> open;
  
 public:

  SymDijkstra(SymManager * manager);

  ~SymDijkstra(){}

  virtual void init_fw();
  virtual void init_bw();
  virtual bool init(const SymExploration & other, int maxTime, int maxNodes);
  virtual bool expand_zero(int maxTime, int maxNodes);
  virtual bool expand_cost(int maxTime, int maxNodes);
  virtual bool pop(int maxTime, int maxNodes);
  virtual bool finished();

  virtual const std::map<int, std::vector<BDD>> & getOpenG() const;
  // virtual std::map <int, std::map <int, std::vector<BDD>>> getOpenGH() const;
};

#endif // SYM_DIJKSTRA
