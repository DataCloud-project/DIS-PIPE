#include "sym_params.h"

#include "../operator.h"
#include "../globals.h"
#include "../option_parser.h"

#include <vector>
using namespace std;

SymParamsMgr::SymParamsMgr(const Options & opts) : 
  cudd_init_nodes(opts.get<long>("cudd_init_nodes")), 
  cudd_init_cache_size(opts.get<long>("cudd_init_cache_size")), 
  cudd_init_available_memory(opts.get<long>("cudd_init_available_memory")), 
  max_tr_size(opts.get<int>("max_tr_size")),
  max_tr_time(opts.get<int>("max_tr_time")),
  mutex_type(MutexType(opts.get_enum("mutex_type"))),
  max_mutex_size(opts.get<int>("max_mutex_size")),
  max_mutex_time(opts.get<int>("max_mutex_time")) {
    //Don't use edeletion with conditional effects
  if(mutex_type == MutexType::MUTEX_EDELETION && domain_has_cond_effects()){
    cout << "Mutex type changed to mutex_and because the domain has conditional effects" << endl;
    mutex_type = MutexType::MUTEX_AND;
  }
}

void SymParamsMgr::print_options() const{
  cout << "CUDD Init: nodes=" << cudd_init_nodes << 
    " cache=" << cudd_init_cache_size << 
    " max_memory=" << cudd_init_available_memory << endl;
  cout << "TR(time=" << max_tr_time << ", nodes=" << max_tr_size << ")" << endl;
  cout << "Mutex(time=" << max_mutex_time << ", nodes=" << max_mutex_size << ", type=" << mutex_type << ")" << endl;
}

void SymParamsMgr::add_options_to_parser(OptionParser &parser){
  parser.add_option<long> ("cudd_init_nodes", 16000000L,
			   "Initial number of nodes in the cudd manager.");

  parser.add_option<long> ("cudd_init_cache_size", 16000000L,
			   "Initial number of cache entries in the cudd manager.");
  
  parser.add_option<long> ("cudd_init_available_memory", 0L,
			   "Total available memory for the cudd manager.");
  
  parser.add_option<int> ("max_tr_size", 100000,
			  "maximum size of TR BDDs");
  
  parser.add_option<int> ("max_tr_time", 60000,
			  "maximum time (ms) to generate TR BDDs");
     
  parser.add_enum_option("mutex_type", MutexTypeValues,
			 "MUTEX_EDELETION",
			 "mutex type");

  parser.add_option<int> ("max_mutex_size", 100000,
			  "maximum size of mutex BDDs");

  parser.add_option<int> ("max_mutex_time", 60000,
			  "maximum time (ms) to generate mutex BDDs");
}









SymParamsSearch::SymParamsSearch(const Options & opts) : 
  max_disj_nodes (opts.get<int>("max_disj_nodes")), 
  min_estimation_time (opts.get<double>("min_estimation_time")), 
  penalty_time_estimation_sum  (opts.get<double>("penalty_time_estimation_sum")),
  penalty_time_estimation_mult (opts.get<double>("penalty_time_estimation_mult")),
  penalty_nodes_estimation_sum (opts.get<double>("penalty_time_estimation_sum")), 
  penalty_nodes_estimation_mult(opts.get<double>("penalty_nodes_estimation_mult")), 
  max_pop_nodes(opts.get<int>("max_pop_nodes")),
  max_pop_time (opts.get<int>("max_pop_time")), 
  maxStepTime  (opts.get<int> ("max_step_time")), 
  maxStepNodes (opts.get<int> ("max_step_nodes")),   
  ratioUseful  (opts.get<double> ("ratio_useful")), 
  minAllotedTime  (opts.get<int>   ("min_alloted_time")),  
  minAllotedNodes (opts.get<int>   ("min_alloted_nodes")), 
  maxAllotedTime  (opts.get<int>   ("max_alloted_time")),
  maxAllotedNodes (opts.get<int>   ("max_alloted_nodes")),
  ratioAllotedTime  (opts.get<double>("ratio_alloted_time")),
  ratioAllotedNodes (opts.get<double>("ratio_alloted_nodes")), 
 ratioAfterRelax (opts.get<double>("ratio_after_relax")){
}

void SymParamsSearch::print_options() const{
  cout << "Disj(nodes=" << max_disj_nodes << ")" << endl;
  cout << "Estimation: min_time(" << min_estimation_time << ")" <<
    " time_penalty +(" << penalty_time_estimation_sum << ")" <<
    "*("  << penalty_time_estimation_mult << ")" <<
    " nodes_penalty +(" << penalty_nodes_estimation_sum << ")" <<
    "*("  << penalty_nodes_estimation_mult << ")" << endl;
  cout << "Pop(time=" << max_pop_time << ", nodes=" << max_pop_nodes << ")" << endl;
  cout << "MaxStep(time=" << maxStepTime << ", nodes=" << maxStepNodes << ")" << endl;
  cout << "Ratio useful: " << ratioUseful << endl;
  cout << "   Min alloted time: " << minAllotedTime << " nodes: " << minAllotedNodes << endl;
  cout << "   Max alloted time: " << maxAllotedTime << " nodes: " << maxAllotedNodes << endl;
  cout << "   Mult alloted time: " << ratioAllotedTime << " nodes: " << ratioAllotedNodes << endl;
  cout << "   Ratio after relax: " << ratioAfterRelax << endl;

}

void SymParamsSearch::add_options_to_parser(OptionParser &parser, int maxStepTime, int maxStepNodes){
  parser.add_option<int> ("max_disj_nodes", numeric_limits<int>::max(),
			  "maximum size to enforce disjunction before image");

  parser.add_option<double> ("min_estimation_time", 1000,
			     "minimum time to perform linear interpolation for estimation");

  parser.add_option<double> ("penalty_time_estimation_sum", 1000,
			     "time added when violated alloted time");
  parser.add_option<double> ("penalty_time_estimation_mult", 2,
			     "multiplication factor when violated alloted time");

  parser.add_option<double> ("penalty_nodes_estimation_sum", 1000,
			     "nodes added when violated alloted nodes");
  parser.add_option<double> ("penalty_nodes_estimation_mult", 2,
			     "multiplication factor when violated alloted nodes");

  parser.add_option<int> ("max_pop_nodes", 1000000,
			  "maximum size in pop operations");
  parser.add_option<int> ("max_pop_time", 2000,
			  "maximum time (ms) in pop operations");

  parser.add_option<int>("max_step_time", maxStepTime,
			 "allowed time to perform a step in the search");
  parser.add_option<int>("max_step_nodes", maxStepNodes,
			 "allowed nodes to perform a step in the search");

  parser.add_option<double>("ratio_useful", 0.0,
			    "Percentage of nodes that can potentially prune in the frontier for an heuristic to be useful");

  //The default value is a 50% percent more than maxStepTime, 
  parser.add_option<int>    ("min_alloted_time", 60e3,
			    "minimum alloted time for an step");
  parser.add_option<int>    ("min_alloted_nodes", 10e6,
			    "minimum alloted nodes for an step");

  parser.add_option<int>    ("max_alloted_time", 60e3,
			    "maximum alloted time for an step");
  parser.add_option<int>    ("max_alloted_nodes", 15e6,
			    "maximum alloted nodes for an step");

  parser.add_option<double> ("ratio_alloted_time", 2.0,
			    "multiplier to decide alloted time for a step");
  parser.add_option<double> ("ratio_alloted_nodes", 2.0,
			    "multiplier to decide alloted nodes for a step");
  parser.add_option<double> ("ratio_after_relax", 0.8, 
			    "multiplier to decide alloted nodes for a step");
}
