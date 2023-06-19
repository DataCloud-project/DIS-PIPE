#include "bhs.h"

#include "sym_hnode.h" 
#include "../debug.h"
#include "sym_exploration.h"

#include "../option_parser.h"
#include "../plugin.h"

BHS::BHS(const Options &opts) : 
  SymEngine(opts),
  maxStepTime       (opts.get<int>   ("max_step_time")), 
  maxStepNodes      (opts.get<int>   ("max_step_nodes")),
  maxStepTimeEnd    (opts.get<int>   ("max_step_time_end")), 
  maxStepNodesEnd   (opts.get<int>   ("max_step_nodes_end")),
  timeEnd           (opts.get<double>   ("time_end")),

  minAllotedTime    (opts.get<int>   ("min_alloted_time")),
  minAllotedNodes   (opts.get<int>   ("min_alloted_nodes")),
  ratioAllotedTime   (opts.get<double>("ratio_alloted_time")),
  ratioAllotedNodes  (opts.get<double>("ratio_alloted_nodes")),

  ratioAbstractTime (opts.get<double>("ratio_abstract_time")),
  ratioAbstractNodes (opts.get<double>("ratio_abstract_nodes")),
  ratioAfterRelaxTime (opts.get<double>   ("ratio_after_relax_time")),
  ratioAfterRelaxNodes(opts.get<double>   ("ratio_after_relax_nodes")),

  relaxDir      (RelaxDirStrategy(opts.get_enum   ("relax_dir"))),
  percentageUseful  (opts.get<double>("p_useful"))/*,
						    forceHeuristic    (opts.get<bool>  ("force_heuristic")), 
						    heuristicAbstract (opts.get<bool>  ("heuristic_abstract"))*/{
}


void BHS::initialize(){
  print_options();
  SymEngine::initialize();
  //if(forceHeuristic){
    //TODO: generate all the hierarchy
  //}
}

int BHS::step(){
  SymExploration * exp =  getExploration();
  
  //TODO: consider alternative exploration
  // int stepTime  = max<int>(min<long>(maxStepTime, exp->nextStepTime()) *stepTimeMult, 
  // 			     minStepTime);
  // int stepNodes = max<int>(min<long>(maxStepNodes, exp->nextStepNodes())*stepNodesMult, 
  // 			   minStepNodes);

  int stepTime  = getAllotedTime(exp->nextStepTime());
  int stepNodes  = getAllotedNodes(exp->nextStepNodes());

  exp->stepImage(stepTime, stepNodes);
  return stepReturn();
}

SymExploration * BHS::
  getExploration(){
  set <SymExploration *> aux;
  set<SymExploration *> usefulExplorations;
  
  //Initialize with explorations of the original state space  
  //TODO: Better way to get the exploration of the original state space
  set<SymExploration *> explorations = originalSearch->getExps();
  
  DEBUG_MSG(cout << "Select exploration" << " total time: " << g_timer << endl;);
  while(!explorations.empty()){
    SymExploration * best = getBestExploration(explorations, false);
    if(best){
      return best;
    }
    //In this iteraton, no exploration was considered explorable
    DEBUG_MSG(cout << "#BHS: No explorable exploration in this iteration " << " total time: " << g_timer << endl;);

    //Store every useful exploration in this level
    usefulExplorations.insert (explorations.begin(),
			       explorations.end());

    for(auto exp : explorations){
      exp->getUsefulExplorations(aux, percentageUseful);
    }

    //explorations.swap(aux);
    set<SymExploration *>().swap(explorations);    
    for(auto exp : aux){
      explorations.insert(exp);
    } 
    set<SymExploration *>().swap(aux);
    DEBUG_MSG(cout << "#BHS: still we have " << explorations.size() << " explorations" << endl;);
  }
  DEBUG_MSG(cout << "#BHS: No valid exploration, add a new exploration" << endl;);

  //No valid exploration according to the parameters
  //We need to add a new exploration 

  //Start with the most abstracted useful explorations 
  for(auto it =usefulExplorations.rbegin();
      it != usefulExplorations.rend(); ++it){    

    SymExploration * exp = *it;
    if(!exp->getBDExp()->isAbstractable()){
      continue;
    }
    DEBUG_MSG(cout << "#BHS: Useful exploration: " << *exp << endl;);
    
    Dir dir = Dir::BIDIR; 
    switch (relaxDir){
    case RelaxDirStrategy::FW: 
      dir = Dir::FW;
      break;
    case RelaxDirStrategy::BW: 
      dir = Dir::BW;
      break;
    case RelaxDirStrategy::BIDIR: 
      dir = Dir::BIDIR;
      break;
    case RelaxDirStrategy::SWITCHBACK:
      if(exp->isFW()){
	dir = Dir::BW;
      }else{
	dir = Dir::FW;	
      }
    }

    cout << "Call to relax" << endl;
    if(saveExploration) {
      exp->getBDExp()->write("save/");
      exit(-1);
    }
    if(!timeBegin){
      timeBegin = g_timer();
    }
    SymBDExp * newBDExp = xxx.relax(exp->getBDExp(), dir,
				   getMaxAfterRelaxTime(),
				   getMaxAfterRelaxNodes(),
				   percentageUseful);

    cout << "Relax done." << newBDExp << endl;
    
    if(newBDExp){
      cout << " >> Abstracted " << *newBDExp << " from " << *exp << " total time: " << g_timer << endl;
      explorations = newBDExp->getExps();      
      return getBestExploration(explorations, true);
    }
  }

  

  DEBUG_MSG(cout << "BHS: I cannot create a new exploration, just search the easiest" << endl;);
  //exit(-1);
  //No valid abstraction according to the parameters
  //Select the exploration estimated to be easiest among all useful ones
  return getBestExploration(usefulExplorations, true);
}

bool BHS::canExplore(const SymExploration & exp) {
  /*  cout << "Exploration abstracted " << exp.isAbstracted() << " forward: " << exp.isFW() << endl;
  if(!exp.isAbstracted() && ((exp.isFW() && searchDir == Dir::BW) || 
			     (!exp.isFW() && searchDir == Dir::FW))){
			     cout << "Forbidden " << endl;

    return false;
    }*/


  return /*exp.getBDExp()->isAbstractable() &&*/ exp.nextStepNodes() <= maxStepNodes &&
    exp.nextStepTime() <= ((double)maxStepTime)*(exp.isAbstracted() ? ratioAbstractTime : 1);
}

bool BHS::isBetter(const SymExploration & exp, const SymExploration & exp2){
  return exp.nextStepTime()/(exp.isAbstracted() ? ratioAbstractTime : 1) < 
    exp2.nextStepTime()/(exp2.isAbstracted() ? ratioAbstractTime : 1);
}

SymExploration * BHS::getBestExploration(set<SymExploration *> & exps,
						bool canExploreAll){
  SymExploration * best = nullptr;
  for (auto exp : exps){
    DEBUG_MSG(cout << "#BHS: Best candidate: " << *exp << endl;);
    if(canExploreAll || canExplore(*exp)){
      if(!best || isBetter(*exp, *best)){
	best = exp;
      }    
    }
  }
  if(best){
    DEBUG_MSG(cout << "#BHS: Best selected: " << *best << endl;);
  }
  /*  else
    cout << "#BHS: Selected nullptr" << endl;*/
  return best;
}


static SymEngine *_parse(OptionParser &parser) {
  SymEngine::add_options_to_parser(parser);

  parser.add_option<int>("max_step_time", 5e3, 
			 "maximum time per step");
  parser.add_option<int>("max_step_nodes", 1e5,
			 "maximum nodes per step");
  parser.add_option<int>("max_step_time_end", 60e3, 
			 "maximum time per step at the end of the execution");
  parser.add_option<int>("max_step_nodes_end", 10e6,
			 "maximum nodes per step at the end of the execution");
  parser.add_option<double>("time_end", 1800, 
			    "number of seconds in the execution");

  parser.add_option<double>("ratio_abstract_time", 0.5,
			 "multiplier to decide alloted time for a step");
  parser.add_option<double>("p_useful", 0.0,
			  "Percentage of nodes that can potentially prune in the frontier for an heuristic to be useful");
  parser.add_option<int>("min_alloted_time", 45e3,
			 "minimum alloted time for an step");
  parser.add_option<int>("min_alloted_nodes", 10e6,
			 "minimum alloted nodes for an step");

  parser.add_option<double>("ratio_alloted_time", 2.0,
			 "ratioiplier to decide alloted time for a step");
  parser.add_option<double>("ratio_alloted_nodes", 2.0,
			 "multiplier to decide alloted nodes for a step");

  parser.add_option<double>("ratio_abstract_time", 0.5,
			 "multiplier to decide alloted time for a step");
  parser.add_option<double>("ratio_abstract_nodes", 1,
			    "multiplier to decide alloted nodes for a step");

  parser.add_option<double>("ratio_after_relax_time", 0.1,
			 "allowed time to accept the abstraction after relaxing the search, compared to the original state space");
  parser.add_option<double>("ratio_after_relax_nodes", 0.5,
			 "allowed nodes to accept the abstraction after relaxing the search, compared to the original state space");

  vector<string> dirs = {"FW", "BW", "BIDIR"};

  parser.add_enum_option("relax_dir", dirs,
			 "BIDIR",
			 "direction allowed to relax");
  parser.add_option<bool>("force_heuristic", false,
			  "forces to compute the heuristic value of every state in the frontier");
  parser.add_option<bool>("heuristic_abstract", false,
			  "If abstract state spaces are allowed to use others as heuristic");
  
  Options opts = parser.parse();
  
  SymEngine *policy = 0;
  if (!parser.dry_run()) {
    policy = new BHS(opts);
  }
  
  return policy;
}
static Plugin<SymEngine> _plugin("bhs", _parse);

static SymEngine *_parse_bd(OptionParser &parser) {
  SymEngine::add_options_to_parser(parser);
  parser.add_option<int>("min_alloted_time", 60000,
			 "minimum alloted time for an step");
  parser.add_option<int>("min_alloted_nodes", 1000000,
			 "minimum alloted nodes for an step");

  parser.add_option<double>("ratio_alloted_time", 2.0,
			 "multiplier to decide alloted time for a step");
  parser.add_option<double>("ratio_alloted_nodes", 2.0,
			 "multiplier to decide alloted nodes for a step");


  Options opts = parser.parse();
  opts.set<int>("max_step_time", numeric_limits<int>::max());
  opts.set<int>("max_step_nodes", numeric_limits<int>::max());
  opts.set<int>("max_step_time_end", numeric_limits<int>::max());
  opts.set<int>("max_step_nodes_end", numeric_limits<int>::max());
  opts.set<double>("time_end", 1800);
  
  opts.set<int>("max_after_relax_time", 0);
  opts.set<int>("max_after_relax_nodes", 0);
  opts.set<int>("max_relax_time", 0);
  opts.set<int>("max_relax_nodes", 0);
  opts.set<double>("ratio_abstract_nodes", 1.0);
  opts.set<double>("ratio_abstract_time", 1.0);
  opts.set<double>("ratio_after_relax_nodes", 1.0);
  opts.set<double>("ratio_after_relax_time", 1.0);

  opts.set("relax_dir", 2);
  opts.set<bool>("force_heuristic", false);
  opts.set<bool>("heuristic_abstract", false);
  opts.set<double>("p_useful", 0.0);

  SymEngine *policy = 0;
  if (!parser.dry_run()) {
    policy = new BHS(opts);
    }
  
  return policy;
}
static Plugin<SymEngine> _plugin_bd("sym_bd", _parse_bd);

//static Plugin<SymEngine> _plugin("sw", _parse);

// SymEngine * BHS::create_default(){
//   Options opts;
//   SymEngine::set_default_options(opts);
//   opts.set<int>("max_step_time", numeric_limits<int>::max());
//   opts.set<int>("max_step_nodes", numeric_limits<int>::max());
//   opts.set<int>("max_step_time_end", numeric_limits<int>::max());
//   opts.set<int>("max_step_nodes_end", numeric_limits<int>::max());
//   opts.set<double>("time_end", 1800);

//   opts.set<double>("ratio_abstract_time", 0.5);
//   opts.set<double>("p_useful", 0.0);
//   opts.set<int>("min_alloted_time", 1000);
//   opts.set<int>("min_alloted_nodes", 100000);
//   opts.set<double>("ratio_alloted_time", 2.0);
//   opts.set<double>("ratio_alloted_nodes", 2.0);
//   opts.set<double>("ratio_abstract_nodes", 1.0);
//   opts.set<double>("ratio_abstract_time", 1.0);
//   opts.set<double>("ratio_after_relax_nodes", 1.0);
//   opts.set<double>("ratio_after_relax_time", 1.0);

//   opts.set<int>("relax_dir", 2);
//   opts.set<bool>("force_heuristic", false);
//   opts.set<bool>("heuristic_abstract", false);
//   return new BHS(opts);
// }



void BHS::print_options() const{
  cout << "PSEL: BHS" << endl;
  cout << "Search dir: ";
  switch (searchDir){
  case Dir::FW: cout << "fw"; break;
  case Dir::BW: cout << "bw"; break;
  case Dir::BIDIR: cout << "bd"; break;
  }
  cout << " relax dir: ";
  switch (relaxDir){
  case RelaxDirStrategy::FW: cout << "fw"; break;
  case RelaxDirStrategy::BW: cout << "bw"; break;
  case RelaxDirStrategy::BIDIR: cout << "bd"; break;
  case RelaxDirStrategy::SWITCHBACK: cout << "sw"; break;
  }
  cout << endl;
  cout << "Max step time: " << maxStepTime << " => " << maxStepTimeEnd << "(first-" << timeEnd <<  ")"<< endl;
  cout << "Max step nodes: " << maxStepNodes << " => " << maxStepNodesEnd << "(first-" << timeEnd <<  ")"<< endl;

  cout << "Min alloted time: " << minAllotedTime << " nodes: " << minAllotedNodes << endl;
  cout << "Mult alloted time: " << ratioAllotedTime << " nodes: " << ratioAllotedNodes << endl;
  //cout << "Max after relax time: " << maxAfterRelaxTime << " nodes: " << maxAfterRelaxNodes << endl;

  cout << "Ratio abstract time: " << ratioAbstractTime << " nodes: " << ratioAbstractNodes << endl;
  cout << "Ratio after relax time: " << ratioAfterRelaxTime <<
    " nodes: " << ratioAfterRelaxNodes << endl;

  cout << "Percentage useful: " << percentageUseful << endl;  
  //cout << "Force heuristic: " << forceHeuristic << endl;
  //  cout << "Heuristic abstract: " << heuristicAbstract << endl << endl;
}


static SymEngine *_parse_test(OptionParser &parser) {
  SymEngine::add_options_to_parser(parser);

  parser.add_option<int>("max_step_time", 1e3, 
			 "maximum time per step");
  parser.add_option<int>("max_step_nodes", 1e3,
			 "maximum nodes per step");

  parser.add_option<double>("ratio_abstract_time", 0.5,
			 "multiplier to decide alloted time for a step");
  parser.add_option<double>("ratio_abstract_nodes", 1,
			    "multiplier to decide alloted nodes for a step");


  parser.add_option<double>("p_useful", 0.0,
			  "Percentage of nodes that can potentially prune in the frontier for an heuristic to be useful");
  parser.add_option<int>("min_alloted_time", 1e3,
			 "minimum alloted time for an step");
  parser.add_option<int>("min_alloted_nodes", 1e3,
			 "minimum alloted nodes for an step");

  parser.add_option<double>("ratio_alloted_time", 2.0,
			 "multiplier to decide alloted time for a step");
  parser.add_option<double>("ratio_alloted_nodes", 2.0,
			 "multiplier to decide alloted nodes for a step");

  /*parser.add_option<int>("max_relax_time", 10e3,
			 "allowed time to relax the search");
  parser.add_option<int>("max_relax_nodes", 5e6,
			 "allowed nodes to relax the search");*/

  parser.add_option<double>("ratio_after_relax_time", 0.1,
			 "allowed time to accept the abstraction after relaxing the search, compared to the original state space");
  parser.add_option<double>("ratio_after_relax_nodes", 0.5,
			 "allowed nodes to accept the abstraction after relaxing the search, compared to the original state space");

  vector<string> dirs = {"FW", "BW", "BIDIR", "SWITCHBACK"};
  parser.add_enum_option("relax_dir", dirs, "BIDIR",
			 "direction allowed to relax");
  parser.add_option<bool>("perimeter_pdbs", true,
			  "initializes explorations with the one being relaxed");
  parser.add_option<bool>("force_heuristic", false,
			  "forces to compute the heuristic value of every state in the frontier");
  parser.add_option<bool>("heuristic_abstract", false,
			  "If abstract state spaces are allowed to use others as heuristic");
  
  Options opts = parser.parse();
  
  SymEngine *policy = 0;
  if (!parser.dry_run()) {
    policy = new BHS(opts);
  }
  
  return policy;
}
static Plugin<SymEngine> _plugin_test("bhst", _parse_test);


double BHS::getMaxStepTime() const {
  if(!timeBegin){
    return maxStepTime;
  }
  double ratio_time = (g_timer() - timeBegin)/(timeEnd - timeBegin);
  return ratio_time*maxStepTime;     
}

double BHS::getMaxStepNodes() const{
  if(!timeBegin){
    return maxStepNodes;
  }
  double ratio_nodes = (g_timer() - timeBegin)/(timeEnd - timeBegin);
  return ratio_nodes*maxStepNodes;
}
