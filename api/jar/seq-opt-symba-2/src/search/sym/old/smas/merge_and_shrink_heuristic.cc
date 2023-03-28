#include "merge_and_shrink_heuristic.h"

#include "symbolic_abstraction.h"
#include "symbolic_exploration/sym_exploration.h"
#include "shrink_fh.h"
#include "variable_order_finder.h"
#include "shrink_bisimulation.h"

#include "../globals.h"
#include "../operator.h"
#include "../option_parser.h"
#include "../plugin.h"
#include "../state.h" 
#include "../timer.h"

#include <cassert>
#include <limits>
#include <vector>
using namespace std;


MergeAndShrinkHeuristic::MergeAndShrinkHeuristic(const Options &opts)
    : Heuristic(opts),
      abstraction_count(opts.get<int>("count")),
      max_tr_size(opts.get<int>("max_tr_size")),
      max_tr_merge_time(opts.get<int>("max_tr_merge_time")),
      max_frontier_size(opts.get<int>("max_frontier_size")),
      max_image_size(max(opts.get<int>("max_image_size"), opts.get<int>("max_frontier_size")*2)),
      max_mas_time(opts.get<int>("max_mas_time")),
      max_sym_time(opts.get<int>("max_sym_time")),
      max_exploration_time(opts.get<int>("max_exploration_time")),
      max_image_time(opts.get<int>("max_image_time")),
      reduce_frontier_factor(opts.get<double>("reduce_frontier_factor")),
      restart_search(opts.get<bool>("restart_search")),
      restart_closed(opts.get<bool>("restart_closed")),
      abstract_transitions(opts.get<bool>("abstract_transitions")),
      merge_strategy(MergeStrategy(opts.get_enum("merge_strategy"))),
      shrink_strategy(opts.get<ShrinkStrategy *>("shrink_strategy")),
      use_h2_mutex(opts.get<bool>("use_h2_mutex")), 
      mutex_type(MutexType(opts.get_enum("mutex_type"))), 
      use_label_reduction(opts.get<bool>("reduce_labels")),
      use_expensive_statistics(opts.get<bool>("expensive_statistics")),
      write_to_disk(opts.get<bool>("write_to_disk")){
}

MergeAndShrinkHeuristic::~MergeAndShrinkHeuristic() {
}

void MergeAndShrinkHeuristic::dump_options() const {

    cout << "Max size: frontier= " << max_frontier_size
	 << ", image=" << max_image_size  
	 << ", transition_relation=" << max_tr_size << endl;

    cout << "Max time: mas= " << max_mas_time
	 << ", sym=" << max_sym_time
	 << ", exploration=" << max_exploration_time
	 << ", image=" << max_image_time 
	 << ", transition_relation=" << max_tr_merge_time << endl;


    cout << "Factor reduce frontier: " << reduce_frontier_factor << endl;

    cout << "Restart search: "
         << (restart_search ? "Yes" : "No") << endl;
    cout << "Restart closed: "
         << (restart_closed ? "Yes" : "No") << endl;

    cout << "Abstract Transitions: "
         << (abstract_transitions ? "Yes" : "No") << endl;

    cout << "Merge strategy: ";
    switch (merge_strategy) {
    case MERGE_LINEAR_CG_GOAL_LEVEL:
        cout << "linear CG/GOAL, tie breaking on level (main)";
        break;
    case MERGE_LINEAR_CG_GOAL_RANDOM:
        cout << "linear CG/GOAL, tie breaking random";
        break;
    case MERGE_LINEAR_GOAL_CG_LEVEL:
        cout << "linear GOAL/CG, tie breaking on level";
        break;
    case MERGE_LINEAR_RANDOM:
        cout << "linear random";
        break;
    case MERGE_DFP:
        cout << "Draeger/Finkbeiner/Podelski" << endl;
        cerr << "DFP merge strategy not implemented." << endl;
        exit(2);
    case MERGE_LINEAR_LEVEL:
        cout << "linear by level";
        break;
    case MERGE_LINEAR_REVERSE_LEVEL:
        cout << "linear by reverse level";
        break;
    default:
        abort();
    }
    cout << endl;
    cout << "H2 Mutexes: " << use_h2_mutex << endl;
    cout << "Mutex type: ";
    switch(mutex_type){
    case MUTEX_NOT:
      cout << " not mutex";
      break;
    case MUTEX_AND:
      cout << "and";
      break;
    case MUTEX_RESTRICT:
      cout << "restrict";
      break;
    case MUTEX_NPAND:
      cout << "non-polluting and";
      break;
    case MUTEX_CONSTRAIN:
      cout << "constrain";
      break;
    case MUTEX_LICOMP:
      cout << "licompaction";
      break;

    }

    cout << endl;

    shrink_strategy->dump_options();
    cout << "Number of abstractions to maximize over: "
         << abstraction_count << endl;
    cout << "Write to disk: " << write_to_disk << endl;
    cout << "Label reduction: "
         << (use_label_reduction ? "enabled" : "disabled") << endl
         << "Expensive statistics: "
         << (use_expensive_statistics ? "enabled" : "disabled") << endl;
}

void MergeAndShrinkHeuristic::warn_on_unusual_options() const {
    if (use_expensive_statistics) {
        string dashes(79, '=');
        cerr << dashes << endl
             << ("WARNING! You have enabled extra statistics for "
            "merge-and-shrink heuristics.\n"
            "These statistics require a lot of time and memory.\n"
            "When last tested (around revision 3011), enabling the "
            "extra statistics\nincreased heuristic generation time by "
            "76%. This figure may be significantly\nworse with more "
            "recent code or for particular domains and instances.\n"
            "You have been warned. Don't use this for benchmarking!")
        << endl << dashes << endl;
    }
}

//Returns if the problem is solved
bool MergeAndShrinkHeuristic::build_abstraction(bool is_first, int * peak_mem) {
    // TODO: We're leaking memory here in various ways. Fix this.
    //       Don't forget that build_atomic_abstractions also
    //       allocates memory.

    int num_heuristics_disk = 0;
    vector<SymbolicAbstraction *> atomic_abstractions;
    SymbolicAbstraction::build_atomic_abstractions(&symbolicManager,
        is_unit_cost_problem(), get_cost_type(), atomic_abstractions);

    ShrinkStrategy *hpreserv_shrink = ShrinkBisimulation::create_default();

    cout << "Shrinking atomic abstractions..." << endl;
    for (size_t i = 0; i < atomic_abstractions.size(); ++i) {
        atomic_abstractions[i]->compute_distances();
        if (!atomic_abstractions[i]->is_solvable()){
	  cerr << "Atomic abstraction " << i << " ";
	  copy(g_fact_names[i].begin(), g_fact_names[i].end(), ostream_iterator<string>(cerr, " "));
	  cerr << "is unsolvable" << endl;
	  
	  return false;
	}

        hpreserv_shrink->shrink_atomic(*atomic_abstractions[i]);
    }

    cout << "Initialize symbolic exploration" << endl;

    std::ostringstream ssPDB; 
    if(write_to_disk){
      ssPDB << "PDB" << ++num_heuristics_disk; 
    }
    SymExploration symExploration(&symbolicManager, max_tr_size,
				  max_tr_merge_time,
				  max_frontier_size, max_image_size,
				  max_image_time, reduce_frontier_factor,
				  restart_search, restart_closed,
				  abstract_transitions, ssPDB.str()); 

    cout << "Merging abstractions..." << endl;

    VariableOrderFinder order(merge_strategy, is_first);

    int var_no = order.next();
    cout << "First variable: #" << var_no << endl;
    SymbolicAbstraction *abstraction = atomic_abstractions[var_no];
    abstraction->statistics(use_expensive_statistics);

    abstraction->set_exploration(&symExploration);

    bool truncatedByTime = false;
    while (!symExploration.finished() && !order.done()) {
      if(g_timer() > max_mas_time){
	truncatedByTime = true;
	break;
      }
      if(symExploration.canExplore() && g_timer() < max_sym_time){
	symExploration.explore(min(max_sym_time - (int)g_timer(), max_exploration_time));
	cout << "Getting heuristic." << endl;
	
	if(!write_to_disk){
	  symExploration.getHeuristic(heuristics, max_heuristic_value); 
	}
	  // heuristics.back().print(1, 1);
	if(symExploration.finished()) break;
      }

        int var_no = order.next();
        cout << "Next variable: #" << var_no << endl;
        SymbolicAbstraction *other_abstraction = atomic_abstractions[var_no];

	//	cout << "Normalize" << endl;
        // TODO: When using nonlinear merge strategies, make sure not
        // to normalize multiple parts of a composite. See issue68.
        if (shrink_strategy->reduce_labels_before_shrinking()) {
            abstraction->normalize(use_label_reduction);
            other_abstraction->normalize(false);
        }
	//	cout << "Compute distances" << endl;
        abstraction->compute_distances();
        if (!abstraction->is_solvable())
            return abstraction;

	
	
	other_abstraction->compute_distances();
	shrink_strategy->shrink_before_merge(*abstraction, *other_abstraction);
    

	// TODO: Make shrink_before_merge return a pair of bools
	//       that tells us whether they have actually changed,
	//       and use that to decide whether to dump statistics?
	// (The old code would print statistics on abstraction iff it was
	// shrunk. This is not so easy any more since this method is not
	// in control, and the shrink strategy doesn't know whether we want
	// expensive statistics. As a temporary aid, we just print the
	// statistics always now, whether or not we shrunk.)
	abstraction->statistics(use_expensive_statistics);
	other_abstraction->statistics(use_expensive_statistics);

	abstraction->normalize(use_label_reduction);
	abstraction->statistics(use_expensive_statistics);
	  
	// Don't label-reduce the atomic abstraction -- see issue68.
	other_abstraction->normalize(false);
	other_abstraction->statistics(use_expensive_statistics);

	//cout << "Merge Symbolic Abstractions" << endl;
	//	abstraction->printBDDs();
	//other_abstraction->printBDDs();
	//Merge
	SymbolicAbstraction *new_abstraction = 
	  new SymbolicAbstraction(&symbolicManager, is_unit_cost_problem(),
				  get_cost_type(),
				  abstraction, other_abstraction);
	//cout << "Merged." << endl;
	
	abstraction->release_memory();
	other_abstraction->release_memory();
	delete abstraction;
	delete other_abstraction;
	abstraction = new_abstraction;
	abstraction->statistics(use_expensive_statistics);
    }


    abstraction->compute_distances();
    if (!abstraction->is_solvable())
      return false;
    
    if(truncatedByTime || order.done()){
      cout << "Adding final heuristic" << endl;
      if(write_to_disk){
	std::ostringstream ssPDB; 
	ssPDB << "PDB" << ++num_heuristics_disk; 
	abstraction->storeExplicitHeuristic(ssPDB.str());
      }else{
	heuristics.push_back(abstraction->getExplicitHeuristic()); 
	max_heuristic_value.push_back(0);
      }

    }
    
    cout << "Finished MaS heuristic. NumADDs = " << heuristics.size() << endl;

    *peak_mem = abstraction->get_peak_memory_estimate();
    abstraction->statistics(use_expensive_statistics);
    abstraction->release_memory();
    delete abstraction;
    return true;
}

void MergeAndShrinkHeuristic::initialize() {
    Timer timer;
    cout << "Initializing merge-and-shrink heuristic..." << endl;
    dump_options();
    warn_on_unusual_options();

    verify_no_axioms_no_cond_effects();
    int peak_memory = 0;

    vector <int> v_order;
    //for(int i = g_variable_name.size() - 1; i >= 0; i--){
    for(int i = 0; i < g_variable_name.size(); i++){
      v_order.push_back(i);
    }
    symbolicManager.init_vars(v_order);

    if(use_h2_mutex){
        // h^2 mutexes are loaded and operators disambiguated
        // pruning ops may lead to finding more mutexes, which may lead to more spurious states
        // actually not worth it, afaik it only works in nomystery
              bool spurious_detected;
        set<pair<int, int> > static_fluents;
	
        do {
            cout << "iteration for mutex detection and operator pruning" << endl;
            spurious_detected = false;
            load_h2_mutexes(-1);

            bool new_unreachable;
            do {
                new_unreachable = false;
                for (int i = 0; i < g_variable_domain.size(); i++) {
                    int count = 0;
                    pair<int,int> static_fluent;
                    for (int j = 0; count < 2 && j < g_variable_domain[i]; j++) {
                        if (!g_unreachable.count(make_pair(i, j))) {
                            count++;
                            static_fluent = make_pair(i, j);
                        }
                    }
                    // if there is only one possible fluent, this fluent is static
                    if (count == 1) {
                        // if it was not detected as unreachable before
                        if (!static_fluents.count(static_fluent)) {
                            static_fluents.insert(static_fluent);
                            set<pair<int, int> > inconsistent_facts = g_inconsistent_facts[static_fluent.first][static_fluent.second];
                            for (set<pair<int, int> >::iterator it = inconsistent_facts.begin(); it != inconsistent_facts.end(); it++) {
                                if (!g_unreachable.count(*it)) {
                                    g_unreachable.insert(*it);
                                    new_unreachable = true;
                                }
                            }
                        }
                    }
                }
            } while(new_unreachable);

            for (int i = 0; i < g_operators.size(); i++)
                if (!g_operators[i].spurious) {
                    g_operators[i].remove_ambiguity();
                    spurious_detected = g_operators[i].spurious || spurious_detected;
                }
        } while (spurious_detected);

	//load_h2_mutexes(-1);
	//for(int i = 0; i < g_operators.size(); i++){
	//g_operators[i].remove_ambiguity();
	//      }
	symbolicManager.init_mutex(g_inconsistent_facts, max_tr_size, mutex_type);
    }
			  
    
    for (int i = 0; i < abstraction_count; i++) {
        cout << "Building abstraction #" << (i + 1) << "..." << endl;
	int peak_memory_abstraction = 0;
        bool is_solvable =  build_abstraction(i == 0, &peak_memory_abstraction);
        peak_memory = max(peak_memory, peak_memory_abstraction);
	//        heuristics.push_back(abstraction->getHeuristicADD());
        if (!is_solvable) {
            cout << "Abstract problem is unsolvable!" << endl;
            if (i + 1 < abstraction_count)
                cout << "Skipping remaining abstractions." << endl;
            break;
        }
    }

    cout << "Done initializing merge-and-shrink heuristic [" << timer << "]" << endl;
    //    debug_plan();
    cout << "Estimated peak memory for abstraction: " << peak_memory << " bytes" << endl;

    if(write_to_disk){
      cout << "BDDs written to disk" << endl;
      exit(0);
    }else{
      cout << "initial h value: " << compute_heuristic(*g_initial_state) << endl;
    }
}

/*void MergeAndShrinkHeuristic::debug_plan(){
    vector <string> operadores;
    operadores.push_back("move-car-to-car car_04 car_01 car_11");
    operadores.push_back("move-curb-to-car car_01 curb_4 car_09");
    operadores.push_back("move-car-to-curb car_04 car_11 curb_4");
    operadores.push_back("move-car-to-car car_12 car_05 car_04");
    operadores.push_back("move-curb-to-car car_05 curb_1 car_11");
    operadores.push_back("move-car-to-curb car_01 car_09 curb_1");
    operadores.push_back("move-curb-to-car car_09 curb_7 car_01");
    operadores.push_back("move-car-to-curb car_07 car_13 curb_7");
    operadores.push_back("move-curb-to-car car_13 curb_2 car_07");
    operadores.push_back("move-car-to-curb car_02 car_10 curb_2");
    operadores.push_back("move-curb-to-car car_10 curb_0 car_02");
    operadores.push_back("move-car-to-curb car_00 car_08 curb_0");
    operadores.push_back("move-curb-to-car car_08 curb_5 car_00");
    operadores.push_back("move-car-to-curb car_05 car_11 curb_5");
    operadores.push_back("move-car-to-car car_13 car_07 car_05");
    operadores.push_back("move-car-to-car car_03 car_06 car_11");
    operadores.push_back("move-curb-to-car car_06 curb_3 car_07");
    operadores.push_back("move-car-to-curb car_03 car_11 curb_3");
    operadores.push_back("move-curb-to-car car_11 curb_6 car_03");
    operadores.push_back("move-car-to-curb car_06 car_07 curb_6");

    State s = g_initial_state;
    for(int op = 0; op < operadores.size(); op++){
      for (int i = 0; i < g_operators.size(); i++){
	if(g_operators[i].get_name() == operadores[op]){
	  s = State(s, g_operators[i]);
	  break;
	}
      }
      cout << operadores[op] <<  "h val: " << compute_heuristic(s);
    }
}
*/


int MergeAndShrinkHeuristic::compute_heuristic(const State &state) {
  int * inputs = symbolicManager.getBinaryDescription(state);
  int cost = 0;
  for (int i = 0; i < heuristics.size(); i++) {
    ADD evalNode = heuristics[i].Eval(inputs);
    int abs_cost = Cudd_V(evalNode.getRegularNode());
    if (abs_cost == -1){
      //cout << "Detected dead end by abstraction " << i << endl;
      return DEAD_END;
    }

    cost = max(cost, abs_cost);

    if (abs_cost < max_heuristic_value[i]){
      //The value in the rest of heuristics should be zero so we can stop
      break; 
    }
  }
  return cost;
}

static ScalarEvaluator *_parse(OptionParser &parser) {
    // TODO: better documentation what each parameter does
    parser.add_option<int>("count", 1, "nr of abstractions to build");
    parser.add_option<int>("max_tr_size", 100000, "maximum number of nodes in the transition relation");
    parser.add_option<int>("max_tr_merge_time", 60, "maximum time merging the transition relation");
    parser.add_option<int>("max_frontier_size", 10000000, "maximum number of nodes in the symbolic search frontier");
    parser.add_option<int>("max_image_size",    20000000, "maximum number of nodes during the image operation. At least max_frontier_size*2");    
    parser.add_option<int>("max_mas_time", 1200, "maximum time (seconds) for  MaS generation. After that, stop heuristic.");
    parser.add_option<int>("max_sym_time", 900, "maximum time (seconds) for Symbolic MaS generation. After that, stop symbolic searches.");
    parser.add_option<int>("max_exploration_time", 300, "maximum time (seconds) each Symbolic exploration.");
    parser.add_option<int>("max_image_time", 30, "maximum time (seconds) for each individual image.");
    parser.add_option<double>("reduce_frontier_factor", 2.0, "factor to divide the size of the frontier when failing during thse image.");
    parser.add_option<bool>("restart_search", false, "If the backward exploration must be restarted when applying shrinking.");
    parser.add_option<bool>("restart_closed", false, "If the backward exploration must be restarted when applying shrinking.");

    parser.add_option<bool>("abstract_transitions", false, "If the transition relations are also abstracted or only state sets.");


    vector<string> merge_strategies;
    //TODO: it's a bit dangerous that the merge strategies here
    // have to be specified exactly in the same order
    // as in the enum definition. Try to find a way around this,
    // or at least raise an error when the order is wrong.
    merge_strategies.push_back("MERGE_LINEAR_CG_GOAL_LEVEL");
    merge_strategies.push_back("MERGE_LINEAR_CG_GOAL_RANDOM");
    merge_strategies.push_back("MERGE_LINEAR_GOAL_CG_LEVEL");
    merge_strategies.push_back("MERGE_LINEAR_RANDOM");
    merge_strategies.push_back("MERGE_DFP");
    merge_strategies.push_back("MERGE_LINEAR_LEVEL");
    merge_strategies.push_back("MERGE_LINEAR_REVERSE_LEVEL");
    parser.add_enum_option("merge_strategy", merge_strategies,
                           "MERGE_LINEAR_CG_GOAL_LEVEL",
                           "merge strategy");


    // TODO: Default shrink strategy should only be created
    // when it's actually used.
    ShrinkStrategy *def_shrink = ShrinkFH::create_default(50000);

    parser.add_option<ShrinkStrategy *>("shrink_strategy", def_shrink, "shrink strategy");
    // TODO: Rename option name to "use_label_reduction" to be
    //       consistent with the papers?
    parser.add_option<bool>("use_h2_mutex", false, "use h2 mutex to prune");


    vector<string> mutex_types;
    //TODO: it's a bit dangerous that the mutex types here
    // have to be specified exactly in the same order
    // as in the enum definition. Try to find a way around this,
    // or at least raise an error when the order is wrong.
    mutex_types.push_back("MUTEX_NOT");
    mutex_types.push_back("MUTEX_AND");
    mutex_types.push_back("MUTEX_RESTRICT");
    mutex_types.push_back("MUTEX_NPAND");
    mutex_types.push_back("MUTEX_CONSTRAIN");
    mutex_types.push_back("MUTEX_LICOMP");
    parser.add_enum_option("mutex_type", mutex_types,
                           "MUTEX_AND",
                           "operation to remove mutex states");


    parser.add_option<bool>("reduce_labels", true, "enable label reduction");
    parser.add_option<bool>("expensive_statistics", false, "show statistics on \"unique unlabeled edges\" (WARNING: "
                            "these are *very* slow -- check the warning in the output)");
    parser.add_option<bool>("write_to_disk", false, "write BDDs to disk and exit");

    Heuristic::add_options_to_parser(parser);
    Options opts = parser.parse();
    if (parser.help_mode())
        return 0;

    MergeStrategy merge_strategy = MergeStrategy(
        opts.get_enum("merge_strategy"));
    if (merge_strategy < 0 || merge_strategy >= MAX_MERGE_STRATEGY) {
        cerr << "error: unknown merge strategy: " << merge_strategy << endl;
        exit(2);
    }

    if (parser.dry_run()) {
        return 0;
    } else {
        MergeAndShrinkHeuristic *result = new MergeAndShrinkHeuristic(opts);
        return result;
    }
}




static Plugin<ScalarEvaluator> _plugin("merge_and_shrink", _parse);

