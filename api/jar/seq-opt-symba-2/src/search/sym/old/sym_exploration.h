#ifndef SYM_EXPLORATION_H
#define SYM_EXPLORATION_H

#include "sym_manager.h"
#include "sym_closed.h"

#include <vector>
#include <map>

/*
 * This class allows to perform a BDD search .
 */

class SymExploration  {  
 protected:
  SymManager * mgr;            //Symbolic manager to perform bdd operations
  bool fw; //Direction of the search. true=forward, false=backward
  SymClosed closed;
  int f, g;            // f and g value of current bucket (S and Szero)
  
  /* 
     Auxiliar zero cost BFS exploration. 
     Allows us to truncate the zero-BFS in cases where it takes too long
     and then we can recover from that.
  */
  BDD S;               // bucket to expand cost transitions
  BDD Szero;           // bucket to expand 0-cost transitions
  //  bool zeroCostExpanded; //If zero cost operators have been expanded substituted by Szero.IsZero()

  //////////////////////////////////////////////////////////////////////////////
 public:

  SymExploration(SymManager * manager);
  ~SymExploration(){}

  

  virtual bool explore(int maxTime, int maxNodes,
		       int maxStepTime, BDD target = BDD(),
		       int fTarget = 0, int gTarget = 0);

  virtual bool stepExpand(int maxTime, int maxNodes);
  virtual bool stepImage(int maxTime, int maxNodes);
  virtual bool stepFDiagonal(int maxTime, int maxNodes);


  virtual void init_fw() = 0; //Init forward search
  virtual void init_bw() = 0; //Init backward search
  //Initialize another search process by reutilizing information of this search
  virtual bool init(const SymExploration & other, int maxTime, int maxNodes) = 0;
  virtual bool finished() = 0;

  /*
   * Apply 0-cost operators over Szero.
   * Puts the result on Szero.
   * Includes the result on S or open (depends on the heuristic value).   
   */
  virtual bool expand_zero(int maxTime, int maxNodes) = 0;

  /*
   * Apply cost-operators over S.
   * Insert S on closed.
   * Insert successors on open. 
   */
  virtual bool expand_cost(int maxTime, int maxNodes) = 0;

  /*
   * Get the next set from the open list and put it on S.
   * If there are 0-cost operators put it on Szero.
   * Update g and f to the current values.
   */
  virtual bool pop(int maxTime, int maxNodes) = 0;

  virtual  const std::map<int, std::vector<BDD>> & getOpenG() const = 0 ;
  //  virtual std::map <int, std::map <int, std::vector<BDD>>> getOpenGH() const = 0;

  inline const SymClosed & getClosed() const{
    return closed;
  }

  /*  inline bool canExplore(){
    return S.nodeCount() <= maxFrontierSize;
    }*/

  inline SymManager * get_mgr(){
    return mgr;
  }

  inline int getF() const{
    return f;
  }

  inline int getG() const{
    return g;
  }

  inline bool getFW() const{
    return fw;
  }

  inline BDD getS() const{
    return S;
  }

};

#endif // SYMBOLIC_EXPLORATION
