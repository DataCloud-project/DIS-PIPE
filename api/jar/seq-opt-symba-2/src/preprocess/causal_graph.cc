/* Computes the levels of the variables and returns an ordering of the variables
 * with respect to their levels, beginning with the lowest-level variable.
 *
 * The new order does not contain any variables that are unimportant for the
 * problem, i.e. variables that don't occur in the goal and don't influence
 * important variables (== there is no path in the causal graph from this variable
 * through an important variable to the goal).
 *
 * The constructor takes variables and operators and constructs a graph such that
 * there is a weighted edge from a variable A to a variable B with weight n+m if
 * there are n operators that have A as prevail condition or postcondition or
 * effect condition for B and that have B as postcondition, and if there are
 * m axioms that have A as condition and B as effect.
 * This graph is used for calculating strongly connected components (using "scc")
 * wich are returned in order (lowest-level component first).
 * In each scc with more than one variable cycles are then eliminated by
 * succesively taking out edges with minimal weight (in max_dag), returning a
 * variable order.
 * Last, unimportant variables are identified and eliminated from the new order,
 * which is returned when "get_variable_ordering" is called.
 * Note: The causal graph will still contain the unimportant vars, but they are
 * suppressed in the input for the search programm.
 */

#include "causal_graph.h"
#include "max_dag.h"
#include "operator.h"
#include "axiom.h"
#include "scc.h"
#include "variable.h"

#include <stdlib.h>
#include <iostream>
#include <cassert>
using namespace std;

bool g_do_not_prune_variables = false;

void CausalGraph::weigh_graph_from_ops(const vector<Variable *> &,
                                       const vector<Operator> &operators,
                                       const vector<pair<Variable *, int> > &) {
    for (int i = 0; i < operators.size(); i++) {
        const vector<Operator::Prevail> &prevail = operators[i].get_prevail();
        const vector<Operator::PrePost> &pre_post = operators[i].get_pre_post();
        vector<Variable *> source_vars;
        for (int j = 0; j < prevail.size(); j++)
            source_vars.push_back(prevail[j].var);
        for (int j = 0; j < pre_post.size(); j++)
            if (pre_post[j].pre != -1)
                source_vars.push_back(pre_post[j].var);

        for (int k = 0; k < pre_post.size(); k++) {
            Variable *curr_target = pre_post[k].var;
            if (pre_post[k].is_conditional_effect)
                for (int l = 0; l < pre_post[k].effect_conds.size(); l++)
                    source_vars.push_back(pre_post[k].effect_conds[l].var);

            for (int j = 0; j < source_vars.size(); j++) {
                Variable *curr_source = source_vars[j];
                WeightedSuccessors &weighted_succ = weighted_graph[curr_source];

                if (predecessor_graph.count(curr_target) == 0)
                    predecessor_graph[curr_target] = Predecessors();
                if (curr_source != curr_target) {
                    if (weighted_succ.find(curr_target) != weighted_succ.end()) {
                        weighted_succ[curr_target]++;
                        predecessor_graph[curr_target][curr_source]++;
                    } else {
                        weighted_succ[curr_target] = 1;
                        predecessor_graph[curr_target][curr_source] = 1;
                    }
                }
            }

            if (pre_post[k].is_conditional_effect)
                source_vars.erase(source_vars.end() - pre_post[k].effect_conds.size(),
                                  source_vars.end());
        }
    }
}

void CausalGraph::weigh_graph_from_axioms(const vector<Variable *> &,
                                          const vector<Axiom> &axioms,
                                          const vector<pair<Variable *, int> > &) {
    for (int i = 0; i < axioms.size(); i++) {
        const vector<Axiom::Condition> &conds = axioms[i].get_conditions();
        vector<Variable *> source_vars;
        for (int j = 0; j < conds.size(); j++)
            source_vars.push_back(conds[j].var);

        for (int j = 0; j < source_vars.size(); j++) {
            Variable *curr_source = source_vars[j];
            WeightedSuccessors &weighted_succ = weighted_graph[curr_source];
            // only one target var: the effect var of axiom[i]
            Variable *curr_target = axioms[i].get_effect_var();
            if (predecessor_graph.count(curr_target) == 0)
                predecessor_graph[curr_target] = Predecessors();
            if (curr_source != curr_target) {
                if (weighted_succ.find(curr_target) != weighted_succ.end()) {
                    weighted_succ[curr_target]++;
                    predecessor_graph[curr_target][curr_source]++;
                } else {
                    weighted_succ[curr_target] = 1;
                    predecessor_graph[curr_target][curr_source] = 1;
                }
            }
        }
    }
}

// This method is called after deriving mutexes and unreachable facts.
// It performs the following steps:
// 1) Remove unneccessary variables
// 2) Reconstruct causal graph with new variables and operators
void CausalGraph::update(){
  cout << "Updating causal graph and pruning unnecessary facts" << endl;

  // 1) Remove unneccessary variables and
  // change ordering to leave out unimportant vars
  vector<Variable *> new_ordering;
  int old_size = ordering.size();
  for (int i = 0; i < old_size; i++)
    if (ordering[i]->is_necessary() || g_do_not_prune_variables){
      new_ordering.push_back(ordering[i]);
      ordering[i]->remove_unreachable_facts();
    }
  ordering = new_ordering;
  for (int i = 0; i < ordering.size(); i++) {
    ordering[i]->set_level(i);
  }
  cout << ordering.size() << " variables of " << old_size << " of " << variables.size() <<  " necessary" << endl;

  // 2) Reconstruct causal graph with new variables and operators

  weighted_graph.clear();
  for (int i = 0; i < ordering.size(); i++){
    weighted_graph[ordering[i]] = WeightedSuccessors();
  }
  weigh_graph_from_ops(ordering, operators, goals);
  weigh_graph_from_axioms(ordering, axioms, goals);
  
  
  //dump();

  //    Partition sccs; atorralba: sccs is now an attribute
  sccs.clear();
  get_strongly_connected_components(ordering, sccs);

  cout << "The causal graph is "
       << (sccs.size() == ordering.size() ? "" : "not ")
       << "acyclic." << endl;
  /*
    if (sccs.size() != variables.size()) {
    cout << "Components: " << endl;
    for(int i = 0; i < sccs.size(); i++) {
    for(int j = 0; j < sccs[i].size(); j++)
    cout << " " << sccs[i][j]->get_name();
    cout << endl;
    }
    }
  */

  //atorralb: For each partition p, complete scc_var
  scc_var.clear();
  for(int p = 0; p < sccs.size(); p++){
    //cout << "scc " << p << ": ";
    for(int v = 0; v < sccs[p].size(); v++){
      scc_var[sccs[p][v]] = p;  
      //cout << sccs[p][v]->get_name() << " ";
    }
    //cout <<endl;
  }

  //calculate_important_vars(); 
  //Put -1 to every variable to remove variables that are not in ordering
  for (int i = 0; i < variables.size(); i++) {
    variables[i]->set_level(-1); 
  }
  //Put right level to each variable
  for (int i = 0; i < ordering.size(); i++) {
    ordering[i]->set_level(i);
  }  
}
CausalGraph::CausalGraph(const vector<Variable *> &the_variables,
                         const vector<Operator> &the_operators,
                         const vector<Axiom> &the_axioms,
                         const vector<pair<Variable *, int> > &the_goals)

    : variables(the_variables), operators(the_operators), axioms(the_axioms),
      goals(the_goals), acyclic(false), rng(RandomNumberGenerator(7)) {
    for (int i = 0; i < variables.size(); i++)
      weighted_graph[variables[i]] = WeightedSuccessors();
    weigh_graph_from_ops(variables, operators, goals);
    weigh_graph_from_axioms(variables, axioms, goals);


    //dump();

    //    Partition sccs; atorralba: sccs is now an attribute
    get_strongly_connected_components(variables, sccs);

    cout << "The causal graph is "
         << (sccs.size() == variables.size() ? "" : "not ")
         << "acyclic." << endl;
    /*
    if (sccs.size() != variables.size()) {
      cout << "Components: " << endl;
      for(int i = 0; i < sccs.size(); i++) {
        for(int j = 0; j < sccs[i].size(); j++)
          cout << " " << sccs[i][j]->get_name();
        cout << endl;
      }
    }
    */

    //atorralb: For each partition p, complete scc_var
    for(int p = 0; p < sccs.size(); p++){
      // cout << "scc " << p << ": ";
      for(int v = 0; v < sccs[p].size(); v++){
	scc_var[sccs[p][v]] = p;  
	// cout << sccs[p][v]->get_name() << " ";
      }
      // cout <<endl;
    }

    calculate_topological_pseudo_sort(sccs);
    calculate_important_vars(); 

    //optimize_ordering_gamer();

    //Put -1 to every variable to remove variables that are not in ordering
    for (int i = 0; i < variables.size(); i++) {
      variables[i]->set_level(-1); 
    }
    //Put right level to each variable
    for (int i = 0; i < ordering.size(); i++) {
      ordering[i]->set_level(i);
    }
}

void CausalGraph::calculate_topological_pseudo_sort(const Partition &sccs) {
    map<Variable *, int> goal_map;
    for (int i = 0; i < goals.size(); i++)
        goal_map[goals[i].first] = goals[i].second;
    for (int scc_no = 0; scc_no < sccs.size(); scc_no++) {
        const vector<Variable *> &curr_scc = sccs[scc_no];
        if (curr_scc.size() > 1) {
            // component needs to be turned into acyclic subgraph

            // Map variables to indices in the strongly connected component.
            map<Variable *, int> variableToIndex;
            for (int i = 0; i < curr_scc.size(); i++)
                variableToIndex[curr_scc[i]] = i;

            // Compute subgraph induced by curr_scc and convert the successor
            // representation from a map to a vector.
            vector<vector<pair<int, int> > > subgraph;
            for (int i = 0; i < curr_scc.size(); i++) {
                // For each variable in component only list edges inside component.
                WeightedSuccessors &all_edges = weighted_graph[curr_scc[i]];
                vector<pair<int, int> > subgraph_edges;
                for (WeightedSuccessors::const_iterator curr = all_edges.begin();
                     curr != all_edges.end(); ++curr) {
                    Variable *target = curr->first;
                    int cost = curr->second;
                    map<Variable *, int>::iterator index_it = variableToIndex.find(target);
                    if (index_it != variableToIndex.end()) {
                        int new_index = index_it->second;
                        if (goal_map.find(target) != goal_map.end()) {
                            //TODO: soll das so bleiben? (Zahl taucht in max_dag auf)
                            // target is goal
                            subgraph_edges.push_back(make_pair(new_index, 100000 + cost));
                        }
                        subgraph_edges.push_back(make_pair(new_index, cost));
                    }
                }
                subgraph.push_back(subgraph_edges);
            }

            vector<int> order = MaxDAG(subgraph).get_result();
            for (int i = 0; i < order.size(); i++) {
                ordering.push_back(curr_scc[order[i]]);
            }
        } else {
            ordering.push_back(curr_scc[0]);
        }
    }
}

void CausalGraph::get_strongly_connected_components(const vector <Variable *> & vars, Partition &result) {
    map<Variable *, int> variableToIndex;
    for (int i = 0; i < vars.size(); i++)
        variableToIndex[vars[i]] = i;

    vector<vector<int> > unweighted_graph;
    unweighted_graph.resize(vars.size());
    for (WeightedGraph::const_iterator it = weighted_graph.begin();
         it != weighted_graph.end(); ++it) {
        int index = variableToIndex[it->first];
        vector<int> &succ = unweighted_graph[index];
        const WeightedSuccessors &weighted_succ = it->second;
        for (WeightedSuccessors::const_iterator it = weighted_succ.begin();
             it != weighted_succ.end(); ++it)
            succ.push_back(variableToIndex[it->first]);
    }

    vector<vector<int> > int_result = SCC(unweighted_graph).get_result();

    result.clear();
    for (int i = 0; i < int_result.size(); i++) {
        vector<Variable *> component;
        for (int j = 0; j < int_result[i].size(); j++)
            component.push_back(vars[int_result[i][j]]);
        result.push_back(component);
    }
}
void CausalGraph::calculate_important_vars() {
    for (int i = 0; i < goals.size(); i++) {
        if (!goals[i].first->is_necessary()) {
            //cout << "var " << goals[i].first->get_name() <<" is directly neccessary."
            // << endl;
            goals[i].first->set_necessary();
            dfs(goals[i].first);
        }
    }
    // change ordering to leave out unimportant vars
    vector<Variable *> new_ordering;
    int old_size = ordering.size();
    for (int i = 0; i < old_size; i++)
        if (ordering[i]->is_necessary() || g_do_not_prune_variables)
            new_ordering.push_back(ordering[i]);
    ordering = new_ordering;
    for (int i = 0; i < ordering.size(); i++) {
        ordering[i]->set_level(i);
    }
    cout << ordering.size() << " variables of " << old_size << " necessary" << endl;
}


void CausalGraph::dfs(Variable *from) {
    for (Predecessors::iterator pred = predecessor_graph[from].begin();
         pred != predecessor_graph[from].end(); ++pred) {
        Variable *curr_predecessor = pred->first;
        if (!curr_predecessor->is_necessary()) {
            curr_predecessor->set_necessary();
            //cout << "var " << curr_predecessor->get_name() <<" is neccessary." << endl;
            dfs(curr_predecessor);
        }
    }
}

const vector<Variable *> &CausalGraph::get_variable_ordering() const {
    return ordering;
}

bool CausalGraph::is_acyclic() const {
    return acyclic;
}

void CausalGraph::dump() const {
    for (WeightedGraph::const_iterator source = weighted_graph.begin();
         source != weighted_graph.end(); ++source) {
        cout << "dependent on var " << source->first->get_name() << ": " << endl;
        const WeightedSuccessors &curr = source->second;
        for (WeightedSuccessors::const_iterator it = curr.begin();
             it != curr.end(); ++it) {
            cout << "  [" << it->first->get_name() << ", " << it->second << "]" << endl;
            //assert(predecessor_graph[it->first][source->first] == it->second);
        }
    }
    for (PredecessorGraph::const_iterator source = predecessor_graph.begin();
         source != predecessor_graph.end(); ++source) {
        cout << "var " << source->first->get_name() << " is dependent of: " << endl;
        const Predecessors &curr = source->second;
        for (Predecessors::const_iterator it = curr.begin();
             it != curr.end(); ++it)
            cout << "  [" << it->first->get_name() << ", " << it->second << "]" << endl;
    }
}
void CausalGraph::generate_cpp_input(ofstream &outfile,
                                     const vector<Variable *> &ordered_vars) const {
    //TODO: use const iterator!
    vector<WeightedSuccessors *> succs; // will be ordered like ordered_vars
    vector<int> number_of_succ; // will be ordered like ordered_vars
    cout << "Number of vars: " << ordered_vars.size() << endl;
    cout << "weighted " << weighted_graph.size() << endl;
    
    succs.resize(ordered_vars.size());
    number_of_succ.resize(ordered_vars.size());
    for (WeightedGraph::const_iterator source = weighted_graph.begin();
         source != weighted_graph.end(); ++source) {
        Variable *source_var = source->first;
        if (source_var->get_level() != -1) {
            // source variable influences others
            WeightedSuccessors &curr = (WeightedSuccessors &)source->second;
	    //cout << "Initialized: " << source_var->get_level() << endl;
            succs[source_var->get_level()] = &curr;
            // count number of influenced vars
            int num = 0;
            for (WeightedSuccessors::const_iterator it = curr.begin();
                 it != curr.end(); ++it)
                if (it->first->get_level() != -1
                    // && it->first->get_level() > source_var->get_level()
                    )
                    num++;
            number_of_succ[source_var->get_level()] = num;
        }
    }

    for (int i = 0; i < ordered_vars.size(); i++) {
        WeightedSuccessors *curr = succs[i];
        // print number of variables influenced by variable i
        outfile << number_of_succ[i] << endl;
        for (WeightedSuccessors::const_iterator it = curr->begin();
             it != curr->end(); ++it) {
            if (it->first->get_level() != -1
                // && it->first->get_level() > ordered_vars[i]->get_level()
                )
                // the variable it->first is important and influenced by variable i
                // print level and weight of influence
                outfile << it->first->get_level() << " " << it->second << endl;
        }
    }
}





void CausalGraph::swap_scc(vector <Variable * > & result){

  vector <int> ini_pos_scc(sccs.size());
  vector <int> scc_order;
  int prev_scc = -1;
  for(int i = 0; i < ordering.size(); i++){
    int scc_i = scc_var[ordering[i]];
    if(scc_i != prev_scc){
      ini_pos_scc[scc_i] = i;
      scc_order.push_back(scc_i);
    }
    prev_scc = scc_i;
  }
  
  int rnd_scc = rng.next(scc_order.size());
  int scc1 = scc_order[rnd_scc];

  int min_pos = rnd_scc;
  int max_pos = rnd_scc;
  while(min_pos > 0             && !conflict_scc[scc1][scc_order[min_pos-1]]) min_pos --; 

  while(max_pos < scc_order.size()-1 && !conflict_scc[scc1][scc_order[max_pos+1]]) max_pos ++; 

  
  
  int rnd_scc2 = min_pos;
  if(min_pos != max_pos) rnd_scc2 += rng.next(max_pos - min_pos);
  int scc2 = scc_order[rnd_scc2];

  //Now, copy ordering in result
  for(int i = 0; i < scc_order.size(); i++){
    int copy_scc = scc_order[i];
    if(copy_scc == scc1){
      copy_scc = scc2;
    }else if(copy_scc == scc2){
      copy_scc = scc1;
    }
    
    for(int j =0; j < sccs[copy_scc].size(); j++){
      result.push_back(ordering[ini_pos_scc[copy_scc] + j]);
    }
  }  
}

/*
void CausalGraph::optimize_ordering(){

  //Initialize influence and conflict between sccs
  conflict_scc = vector <vector <bool> > (sccs.size(),
					  vector <bool> (sccs.size(), false));

  for(int i = 0; i < variables.size() -1 ; i++){   
    Variable * v1 = variables[i];
    WeightedSuccessors &successors = weighted_graph[v1];
    for(WeightedSuccessors::iterator it = successors.begin(); it != successors.end(); ++it) {
      Variable * v2 = it->first;
      int w = it->second;
      if(w > 0){ 
	cout << "There is conflict between " << v1->get_name()
	     << " and "<< v2->get_name() << ": " << w << endl;
	conflict_scc[scc_var[v2]][scc_var[v1]] = true;
      }
    }
  }

  long value_optimization_function, new_value;
  for (int counter = 0; counter < 100; counter++) {
    value_optimization_function = optimize_variable_ordering(ordering, 1000);
    vector <Variable *> new_order; //Copy the order
    swap_scc(new_order); //Swap scc
    new_value = optimize_variable_ordering(new_order, 1000); 
    if (new_value < value_optimization_function){
      value_optimization_function = new_value;
      ordering = new_order;
    }
  }
  optimize_variable_ordering(ordering, 10000);
 }*/


void CausalGraph::optimize_ordering_gamer(){

  cout << "Ordering: " << ordering.size() << endl;
  for (int i = 0; i < ordering.size(); i++) {
    influence_graph.push_back(vector <bool> (ordering.size(), false));
    ordering[i]->set_level(i);     
  }

  influence_graph_from_ops(ordering, operators, goals);    

  cout << "previous variable order: ";
  for(int i = 0; i < ordering.size(); i++)
    cout << ordering[i]->get_name()<<" - ";
  cout << ": " << compute_function(ordering) << endl;

  long value_optimization_function = optimize_variable_ordering_gamer(ordering, 50000);
  cout << "New value: " << compute_function(ordering) << endl;


  for (int counter = 0; counter < 20; counter++) {
    vector <Variable *> new_order; //Copy the order
    for(int i = 0; i < ordering.size(); i++){
      int rnd_pos = rng.next(ordering.size() - i);
      int pos = -1;
      do{       
	pos++;
	bool found;
	do {
	  found = false;
	  for(int j = 0; j < new_order.size(); j++){
	    if(new_order[j] == ordering[pos]){
	      found = true;
	      break;
	    }
	  } 
	  if(found) pos++;
	}while(found);
      }while(rnd_pos-- > 0);
      new_order.push_back(ordering[pos]);
    }

    //cout << "Init value: " << compute_function(new_order) << endl;
    long new_value = optimize_variable_ordering_gamer(new_order, 50000); 

    if (new_value < value_optimization_function){
      value_optimization_function = new_value;
      ordering = new_order;
      cout << "New value: " <<  value_optimization_function << endl;
    }
  }
  //cout << "final distance: " << compute_function(ordering) << endl;
  // exit(-1);
  //optimize_variable_ordering(ordering, 10000);


  cout << "new variable order: ";
  for(int i = 0; i < ordering.size(); i++)
    cout << ordering[i]->get_name()<<" - ";
    
  cout << ": " << compute_function(ordering) << endl;

  //Put right level to each variable
  for (int i = 0; i < ordering.size(); i++) {
    ordering[i]->set_level(i);
  }
}


/*long CausalGraph::optimize_variable_ordering(vector <Variable *> & order,  int iterations){
    long totalDistance = compute_function(order);
  for(int i = 0; i < order.size(); i++){
      for(int j = 0; j < order.size(); j++){
	if(influence(order[i], order[j])){
	  cout << order[i]->get_name() << " influences " << order[j]->get_name() << ": " << influence(order[i], order[j]) <<  endl;
	}
      }
  }
  cout << "initiale distance: " << totalDistance << endl;

  long oldTotalDistance = totalDistance;
  //Repeat iterations times
  for (int counter = 0; counter < iterations; counter++) {

    //Swap variable
    int rnd_var = rng.next(order.size());
    int scc_rnd_var = scc_var[order[rnd_var]];
    int ini_scc_var = rnd_var;
    while(ini_scc_var > 0 && scc_var[order[ini_scc_var -1]] == scc_rnd_var) ini_scc_var--;

    int swapIndex1 = rnd_var; 
    int swapIndex2 = ini_scc_var + rng.next(sccs[scc_rnd_var].size());

    //Compute the new value of the optimization function
    for(int i = 0; i < order.size(); i++){
      totalDistance = totalDistance + 
	influence(order[i], order[swapIndex1])
	*(- (i - swapIndex1)*(i - swapIndex1)
	  + (i - swapIndex2)*(i - swapIndex2));
	
      totalDistance = totalDistance +
	influence(order[i], order[swapIndex2])*
	(- (i - swapIndex2)*(i - swapIndex2)
	 + (i - swapIndex1)*(i - swapIndex1));	
    }

    //Apply the swap if it is worthy
    if (totalDistance < oldTotalDistance){
      Variable * tmp = order[swapIndex1];
      order[swapIndex1] = order[swapIndex2];
      order[swapIndex2] = tmp;
      oldTotalDistance = totalDistance;
    }else{
      totalDistance = oldTotalDistance;
    }
    }

    return totalDistance;
  return 0;
  }*/


long CausalGraph::optimize_variable_ordering_gamer(vector <Variable *> & order,  int iterations){
  long totalDistance = compute_function(order);
    
  /*for(int i = 0; i < order.size(); i++){
    for(int j = 0; j < order.size(); j++){
      if(influence(order[i], order[j])){
	cout << order[i]->get_name() << " influences " << order[j]->get_name() << ": " << influence(order[i], order[j]) <<  endl;
      }
    }
    }*/
  //  cout << "initiale distance: " << totalDistance << endl;

  long oldTotalDistance = totalDistance;
  //Repeat iterations times
  for (int counter = 0; counter < iterations; counter++) {
    //    cout << "Counter: " << counter << endl;
    //Swap variable
    int swapIndex1 = rng.next(order.size());
    int swapIndex2 = rng.next(order.size());
    //cout << "SWAP: " << swapIndex1 << " with " << swapIndex2 << endl;
    if(swapIndex1 == swapIndex2) continue;

    //Compute the new value of the optimization function
    for(int i = 0; i < order.size(); i++){
      if(i == swapIndex1 || i == swapIndex2) continue;

      if(influence(order[i], order[swapIndex1]))
	totalDistance += (- (i - swapIndex1)*(i - swapIndex1)
			  + (i - swapIndex2)*(i - swapIndex2));
	
      if(influence(order[i], order[swapIndex2]))
	totalDistance += (- (i - swapIndex2)*(i - swapIndex2)
			  + (i - swapIndex1)*(i - swapIndex1));	
    }

    //Apply the swap if it is worthy
    if (totalDistance < oldTotalDistance){
      Variable * tmp = order[swapIndex1];
      order[swapIndex1] = order[swapIndex2];
      order[swapIndex2] = tmp;
      oldTotalDistance = totalDistance;
      
      /*if(totalDistance != compute_function(order)){
	cerr << "Error computing total distance: " << totalDistance << " " << compute_function(order) << endl;
	exit(-1);
      }else{
	cout << "Bien: " << totalDistance << endl;
      }*/
    }else{
      totalDistance = oldTotalDistance;
    }
  }
//  cout << "Total distance: " << totalDistance << endl;
  return totalDistance;
}


long CausalGraph::compute_function(vector <Variable *> & order){
  long totalDistance = 0;
  for (int i = 0; i < order.size() - 1; i++) {
    for (int j = i+1; j < order.size(); j++) {
      if(influence(order[i], order[j])){
	totalDistance += (i-j)*(i -j);
      }
    }
  }
  return totalDistance;
}

long CausalGraph::influence(Variable * v1, Variable * v2){
  //long w = 0;
  //WeightedSuccessors::iterator res = weighted_graph[v1].find(v2);
  //if(res != weighted_graph[v1].end()) w += res->second;

  //res = weighted_graph[v2].find(v1);
  //if(res != weighted_graph[v2].end()) w += res->second;

  //if(w > 0) return 1;
  //else return w;
  /*long w = 0;

  WeightedSuccessors::iterator res = influence_graph[v1].find(v2);
  if(res != influence_graph[v1].end()) w += res->second;

  res = influence_graph[v2].find(v1);
  if(res != influence_graph[v2].end()) w += res->second;

  if(w != 0) return 1;
  else return 0;*/
  return influence_graph[v1->get_level()] [v2->get_level()] ||
    influence_graph[v2->get_level()] [v1->get_level()];
}



void CausalGraph::influence_graph_from_ops(const vector<Variable *> &,
					   const vector<Operator> &operators,
					   const vector<pair<Variable *, int> > &) {
  for (int i = 0; i < operators.size(); i++) {
    if(operators[i].is_redundant()){
      continue;
    }
    const vector<Operator::Prevail> &prevail = operators[i].get_prevail();
    const vector<Operator::PrePost> &pre_post = operators[i].get_pre_post();
    vector<Variable *> pre_vars;
    vector<Variable *> eff_vars;
    for (int j = 0; j < prevail.size(); j++)
      pre_vars.push_back(prevail[j].var);
    for (int j = 0; j < pre_post.size(); j++){
      eff_vars.push_back(pre_post[j].var);
      if (pre_post[j].is_conditional_effect)
	for (int k = 0; k < pre_post[j].effect_conds.size(); k++)
	  pre_vars.push_back(pre_post[j].effect_conds[k].var);
    }
	
    for (int j = 0; j < pre_vars.size(); j++){
      int vpre = pre_vars[j]->get_level();
      for (int k = 0; k < eff_vars.size(); k++) {
	int veff = eff_vars[k]->get_level();
	if (vpre != veff){ 
	  influence_graph[vpre][veff] = true;
	  influence_graph[veff][vpre] = true;
	}
      }
    }

    for (int j = 0; j < eff_vars.size(); j++) {
      for (int k = 0; k < eff_vars.size(); k++) {
	if (eff_vars[j] != eff_vars[k]) {
	  influence_graph[eff_vars[j]->get_level()][eff_vars[k]->get_level()] = true;
	  influence_graph[eff_vars[k]->get_level()][eff_vars[j]->get_level()] = true;
	}      
      }
    }
  }
}
