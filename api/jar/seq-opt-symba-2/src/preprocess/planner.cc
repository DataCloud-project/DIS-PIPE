/* Main file, keeps all important variables.
 * Calls functions from "helper_functions" to read in input (variables, operators,
 * goals, initial state),
 * then calls functions to build causal graph, domain_transition_graphs and
 * successor generator
 * finally prints output to file "output"
 */

#include "helper_functions.h"
#include "successor_generator.h"
#include "causal_graph.h"
#include "domain_transition_graph.h"
#include "state.h"
#include "mutex_group.h"
#include "operator.h"
#include "axiom.h"
#include "h2_mutexes.h"
#include "variable.h"
#include <iostream>
using namespace std;

int main(int argc, const char ** argv) {
  int h2_mutex_time = 300; // 5 minutes to compute mutexes by default
  bool optimize_ordering = false;  
  bool write_to_file_for_gamer = false;

  
  bool metric;
  vector<Variable *> variables;
  vector<Variable> internal_variables;
  State initial_state;
  vector<pair<Variable *, int> > goals;
  vector<MutexGroup> mutexes;
  vector<Operator> operators;
  vector<Axiom> axioms;
  vector<DomainTransitionGraph> transition_graphs;

  for (int i = 1; i < argc; ++i) {
    string arg = string(argv[i]);
    if(arg.compare("--opt_ordering") == 0){
      optimize_ordering = true;
    }else if (arg.compare("--cgamer") == 0){
      write_to_file_for_gamer = true;
    }else if (arg.compare("--not_prune") == 0){
      cout << "*** do not perform relevance analysis ***" << endl;
      g_do_not_prune_variables = true;
    }else{
      cerr << "unknown option " << arg << endl << endl;
      cout << "Usage: ./preprocess [--cgamer] [--opt_ordering] < output"<< endl;
      exit(2);
    }
  }

  read_preprocessed_problem_description
    (cin, metric, internal_variables, variables, mutexes, initial_state, goals, operators, axioms);
  //dump_preprocessed_problem_description
  //  (variables, initial_state, goals, operators, axioms);


  cout << "Building causal graph..." << endl;
  CausalGraph causal_graph(variables, operators, axioms, goals);
  const vector<Variable *> &ordering = causal_graph.get_variable_ordering();
  bool cg_acyclic = causal_graph.is_acyclic();

  // Remove unnecessary effects from operators and axioms, then remove
  // operators and axioms without effects.
  strip_mutexes(mutexes);
  strip_operators(operators);
  strip_axioms(axioms);

  // compute h2 mutexes
  if(h2_mutex_time){
    compute_h2_mutexes(ordering, operators, axioms,
		       mutexes, initial_state, goals, 
		       h2_mutex_time);
    //Update the causal graph and remove unneccessary variables
    //TODO: remove variables
    strip_mutexes(mutexes);
    strip_operators(operators);
    strip_axioms(axioms);      

    cout << "Change id of operators: " << operators.size() << endl;
    // 1) Change id of values in operators and axioms to remove unreachable facts from variables
    for(int i = 0; i < operators.size(); ++i){
      operators[i].remove_unreachable_facts();
    }
    cout << "Change id of axioms: " << axioms.size() << endl;
    for(int i = 0; i < axioms.size(); ++i){
      axioms[i].remove_unreachable_facts();
    }
    cout << "Change id of mutexes" << endl;
    for(int i = 0; i < mutexes.size(); ++i){
      mutexes[i].remove_unreachable_facts();
    }
    cout << "Change id of goals" << endl;
    vector<pair<Variable *, int> > new_goals;
    for(int i = 0; i < goals.size(); ++i){
      if(goals[i].first->is_necessary()){
	goals[i].second = goals[i].first->get_new_id(goals[i].second);
	new_goals.push_back(goals[i]);
      }
    }
    new_goals.swap(goals);
    cout << "Change id of initial state" << endl;
    initial_state.remove_unreachable_facts();


    cout << "Remove unreachable facts from variables: " << ordering.size() << endl;
    // 2)Remove unreachable facts from variables
    for (int i = 0; i < ordering.size(); ++i){
      if (ordering[i]->is_necessary()){
	ordering[i]->remove_unreachable_facts();
      }
    }
      
    strip_mutexes(mutexes);
    strip_operators(operators);
    strip_axioms(axioms);      

    causal_graph.update();
    cg_acyclic = causal_graph.is_acyclic();
    strip_mutexes(mutexes);
    strip_operators(operators);
    strip_axioms(axioms);
  }

  //Merge mutexes and make exactly one invariant groups
  generate_invariants(mutexes, operators, initial_state); 
    
  if(optimize_ordering){
    causal_graph.optimize_ordering_gamer();
  }

  cout << "Building domain transition graphs..." << endl;
  build_DTGs(ordering, operators, axioms, transition_graphs);
  //dump_DTGs(ordering, transition_graphs);
  bool solveable_in_poly_time = false;
  if (cg_acyclic)
    solveable_in_poly_time = are_DTGs_strongly_connected(transition_graphs);
  /*
    TODO: The above test doesn't seem to be quite ok because it
    ignores axioms and it ignores non-unary operators. (Note that the
    causal graph computed here does *not* contain arcs between
    effects, only arcs from preconditions to effects.)

    So solveable_in_poly_time [sic] should also be set to false if
    there are any derived variables or non-unary operators.
  */

  //TODO: genauer machen? (highest level var muss nicht scc sein...gemacht)
  //nur Werte, die wichtig sind fuer drunterliegende vars muessen in scc sein
  cout << "solveable in poly time " << solveable_in_poly_time << endl;
  cout << "Building successor generator..." << endl;
  SuccessorGenerator successor_generator(ordering, operators);
  //successor_generator.dump();


  // Output some task statistics
  int facts = 0;
  int derived_vars = 0;
  for (int i = 0; i < ordering.size(); i++) {
    facts += ordering[i]->get_range();
    if (ordering[i]->is_derived())
      derived_vars++;
  }
  cout << "Preprocessor facts: " << facts << endl;
  cout << "Preprocessor derived variables: " << derived_vars << endl;

  // Calculate the problem size
  int task_size = ordering.size() + facts + goals.size();

  for (int i = 0; i < mutexes.size(); i++)
    task_size += mutexes[i].get_encoding_size();

  for (int i = 0; i < operators.size(); i++)
    task_size += operators[i].get_encoding_size();

  for (int i = 0; i < axioms.size(); i++)
    task_size += axioms[i].get_encoding_size();

  cout << "Preprocessor task size: " << task_size << endl;

  cout << "Writing output..." << endl;
  generate_cpp_input(solveable_in_poly_time, ordering, metric,
		     mutexes, initial_state, goals,
		     operators, axioms, successor_generator,
		     transition_graphs, causal_graph);
  if(write_to_file_for_gamer){
    write_mutexes(variables, mutexes, "file_mutexes");
    write_operators(operators, "file_operators");
    write_variables(variables, "file_variables");
    }
  cout << "done" << endl << endl;
}
