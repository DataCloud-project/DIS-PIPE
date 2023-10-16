#include "sym_exploration.h"

#include "sym_abstraction.h"

#include "../timer.h"
#include "../globals.h"
#include <sstream>
#include <iostream>
#include <fstream>
#include <string>


bool SymExploration::explore(int maxTime, int maxNodes,
			     int maxStepTime, BDD target,
			     int fTarget, int gTarget){
  /*cout << "Starting Symbolic Exploration Open size: " 
    << open.size() << endl;*/

  Timer t;
  while(!finished()){
    if(f >= fTarget || g >= gTarget){
      return true;
    }
    if(!target.IsZero()){
      target = target*closed.notClosed();
      if(target.IsZero()){
	return true;
      }
    }
      
    if(!stepImage(std::min((double)maxStepTime, maxTime - t()), 
		  maxNodes)){
      //If the step was interrupted, stop the exploration
      return false; 
    }
  }
  return true;
}

bool SymExploration::stepExpand(int maxTime, int maxNodes){
  if(maxTime <= 0) return false;
  Timer t;
  while(!Szero.IsZero()){
    if(!stepImage(maxTime - t(), maxNodes)){
      return false;
    }
  }
  if(!stepImage(maxTime - t(), maxNodes)){
    return false;
  }
  return true;
}


bool SymExploration::stepImage(int maxTime, int maxNodes){
  if(maxTime <= 0) return false;
  if(S.IsZero() && Szero.IsZero()){
    if(!pop(maxTime, maxNodes)){
      return false;
    }
  }

  if(!Szero.IsZero()){
    //Image with respect to 0-cost  actions
    if(Szero.nodeCount() > maxNodes){
      cout << "Truncated. 0-Frontier size exceeded: " 
	   << Szero.nodeCount() << endl;
      return false;
    }
    cout << "  0-cost step: " << Szero.nodeCount() << " nodes. " << flush;
    return expand_zero(maxTime, maxNodes);
  }else if(S.IsZero()){
    //Image with respect to cost actions
    if(S.nodeCount() > maxNodes){
      cout << "Truncated. Cost-Frontier size exceeded: " 
	   << S.nodeCount() << endl;
      return false;
    }
    cout << " cost step: " << S.nodeCount() << " nodes. " << flush;
    return expand_cost(maxTime, maxNodes);
  }

  return false;
}

bool SymExploration::stepFDiagonal(int maxTime, int maxNodes){
  Timer t; 
  int current_f = f;
  while(current_f == f){
    if(!stepExpand(maxTime - t(), maxNodes)){
      return false;
    }
  }

  return true;
}


SymExploration::SymExploration(SymManager * manager): mgr(manager), fw(true), closed(manager), f(0), g(0) {}

