#include "sym_dijkstra.h"

#include "../timer.h"
#include <ext/hash_map> 

using namespace __gnu_cxx;

SymDijkstra::SymDijkstra(SymManager * manager) : SymExploration(manager){
}


void SymDijkstra::init_fw(){
  f = 0;
  g = 0;
  open[0] = vector<BDD>();
  open[0].push_back(mgr->getInitialState());
  fw = true;
  
}

void SymDijkstra::init_bw(){
  f = 0;
  g = 0;
  open[0] = vector<BDD>();
  open[0].push_back(mgr->getGoal());
  fw = false;  
}

bool SymDijkstra::init(const SymExploration & other, int  maxTime, int  maxNodes){
  f = other.getF();
  g = other.getG();
  fw = other.getFW();

  //Shrink S, Szero
  try{
    S = mgr->shrinkExists(other.getS(), maxTime, maxNodes);
    Szero = S;
  }catch(BDDError e){
    return false;
  }
  
  if (S.nodeCount() > maxNodes){ //We are not going to use this exploration anyway
    return false;
  }
  try{ 
    //Remove closed states from open and shrink it.
    //This can have prunning power because it is performed before the shrinking.
    for(auto openIt : other.getOpenG()){
      open[openIt.first] = vector<BDD>();
      const vector<BDD> & openF = openIt.second;
      for(BDD bdd : openF){
	BDD shrinkedBDD = mgr->shrinkExists(bdd*closed.notClosed(), maxTime, maxNodes);
	open[openIt.first].push_back(shrinkedBDD);
      }
    }

    //Shrink closed
    closed.shrink(other.getClosed(), mgr->getAbstraction() );
  }catch(BDDError e){
    return false;
  }
  

  return true;
}

bool SymDijkstra::finished(){
  return open.empty() && Szero.IsZero() && S.IsZero();
}

bool SymDijkstra::pop(int maxTime, int maxNodes){
 
  //mgr->mgr().SetTimeLimit(maxTime*1000);
  //mgr->mgr().ResetStartTime();

  g = open.begin()->first;
  
  /*if(!mgr->apply(open[g], mgr->shrink)){
    cout << "Truncated while shrinking in pop_open." << endl; 
    return false;
  }*/
  try{
    auto mergeBDDs = [] (BDD bdd, BDD bdd2, int maxNodes){ /*res = res.Or(res_preimage, maxNodesImage)*/
      return bdd.Or(bdd2, maxNodes);
    };
    mgr->getVars()->merge(open[g], mergeBDDs, maxTime, maxNodes);
  }catch(BDDError e){
    cout << "Truncated while pop_open disjunction." << endl; 
    return false;   
  }
  BDD open_bdd = open[g][0];
  open.erase(g); 
  //mgr->mgr().UnsetTimeLimit();

  Szero = open_bdd*closed.notClosed();

  BDD filteredSzero; 
  if(!mgr->filter_mutex(Szero, filteredSzero, maxTime, maxNodes)){
    cout << "Truncated while filtering mutex: Szero "<< Szero.nodeCount() << endl;
    return false;
  }
  Szero = filteredSzero;
  S = Szero;
  closed.insert(g, Szero);
  cout << "Iteration: " << g 
       << " frontier size: " << S.nodeCount() 
       << " total time: " << g_timer << endl;
  return true;
}

bool SymDijkstra::expand_cost(int maxTime, int maxNodes){
  hash_map<int, vector<BDD> > resImage;
  if(!mgr->cost_preimage(S, resImage, maxTime, maxNodes)){
    //maxFrontierSize = min(maxFrontierSize, (int)(S.nodeCount()/reduceFrontierFactor));
    //cout << "Truncated during preimage computation. " 
    //	 << " MaxFrontierSize set to "<< maxFrontierSize << endl;
	
      return false;
    }

    S = mgr->zeroBDD();

    //Include new states in the open list 
    for(hash_map<int, vector<BDD> >::const_iterator it = resImage.begin();
	it != resImage.end(); ++it){
      int cost = g + it->first;
      const vector<BDD> &res_preimages = it->second;
      for(vector<BDD>::const_iterator it2 = res_preimages.begin();
	  it2 != res_preimages.end(); ++it2){		
	if(!((*it2).IsZero())){
	  if(!open.count(cost)){
	    open[cost] = vector<BDD>();
	  }
	  open[cost].push_back(*it2);	  
	}
      }
    }

    return true;
}

bool SymDijkstra::expand_zero(int maxTime, int maxNodes){
  BDD res_preimage;
  try{
    res_preimage = mgr->zero_preimage(Szero, maxTime, maxNodes);
  }catch(BDDError e){
    //maxFrontierSize /= reduceFrontirFactor;
    /*maxFrontierSize = min(maxFrontierSize,
			  (int) (Szero.nodeCount()/reduceFrontierFactor));

    << " MaxFrontierSize set to "<< maxFrontierSize << endl;*/
    cout << "Truncated during 0-image computation." << endl;
    return false;
  }
  Szero = res_preimage*closed.notClosed();
  BDD filteredSzero; 
  if(!mgr->filter_mutex(Szero, filteredSzero, maxTime, maxNodes)){
    cout << "Truncated while filtering mutex" << endl;
    return false;
  }
  Szero = filteredSzero; 
  /*cout << " 0-Step took " << img_timer << " exp time: " << exp_timer 
    << " total time: " << g_timer << endl;*/
  if(!Szero.IsZero()){
    S += Szero;
    closed.insert(g, Szero);
  }
  return true;
}



/*SymDijkstra::getOpenGH() const
{
  return nullptr;
  }*/

const std::map<int, std::vector<BDD>> & SymDijkstra::getOpenG() const
{
  return open;
}
