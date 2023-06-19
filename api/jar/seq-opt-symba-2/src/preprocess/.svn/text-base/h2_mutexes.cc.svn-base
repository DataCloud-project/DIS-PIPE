/* Implemented by Vidal Alcazar Saiz. */

#include "h2_mutexes.h"

//#include "utilities.h"

#include <algorithm>
#include <vector>
#include <set>

using namespace std;

Op_h2::Op_h2(const Operator & op,
	     const vector< vector<unsigned> > & p_index, 
	     const vector<vector<set<pair<int, int> > > > & inconsistent_facts,
	     bool regression){
  // //cout << "New op: " << op.get_name() << endl;

  if(op.is_redundant()){
    triggered = SPURIOUS;
  }else{
    triggered = NOT_REACHED;
  }

  if(regression){
    instantiate_operator_backward(op, p_index, inconsistent_facts);
  }else{
    instantiate_operator_forward(op, p_index, inconsistent_facts);
  }

  sort(pre.begin(), pre.end());
  sort(add.begin(), add.end());
  sort(del.begin(), del.end());

  vector<unsigned int> aux;
  set_difference (del.begin(), del.end(), add.begin(), add.end(), back_inserter(aux));
  del.swap(aux);
  sort(del.begin(), del.end());
}

void compute_h2_mutexes(const vector <Variable *> &variables,
			vector<Operator> &operators,
			vector<Axiom> & axioms,
			vector<MutexGroup> & mutexes,
			State & initial_state,
			const vector<pair<Variable *, int> > & goals,
			int limit_seconds){

  H2Mutexes h2 (limit_seconds);

  if(!h2.initialize(variables, mutexes)){
    return;
  }
  // h^2 mutexes are loaded and operators disambiguated
  // pruning ops may lead to finding more mutexes, which may lead to more spurious states
  // actually not worth it, afaik it only works in nomystery
  bool update_progression = true;
  bool update_regression = true;
  bool regression = false;
  clock_t start_t = clock();
  do {
    if((!regression && update_progression) ||
       (regression && update_regression)){
      if (regression) {
	update_regression = false;
      } else {
	update_progression = false;
      }
      cout << "iteration for mutex detection and operator pruning" << endl;
      int mutexes_detected = h2.compute(variables, operators, axioms, initial_state, goals, mutexes, regression);
      if (mutexes_detected == -1) {
	break;
      }
      h2.detect_unreachable_fluents(variables);    

      bool spurious_detected = h2.remove_spurious_operators(operators);
      update_progression |= spurious_detected || (regression && mutexes_detected);
      update_regression |= spurious_detected || (!regression && mutexes_detected);
    }
    regression = !regression;
  } while (update_progression || update_regression);  
  cout << "Total mutex and disambiguation time: " << (double)(clock() - start_t) / CLOCKS_PER_SEC << endl;

}

void H2Mutexes::detect_unreachable_fluents(const vector <Variable *> &variables){
  bool new_unreachable;
  do {
    new_unreachable = false;
    for (int i = 0; i < num_vars; i++) {
      int count = 0;
      pair<int,int> static_fluent;
      for (int j = 0; count < 2 && j < num_vals[i]; j++) {
	if (!is_unreachable(i, j)) {
	  count++;
	  static_fluent = make_pair(i, j);
	}
      }
      // if there is only one possible fluent, this fluent is static
      if (count == 1) {
	// if it was not detected as unreachable before
	if (!unreachable_fluents.count(static_fluent)) {
	  unreachable_fluents.insert(static_fluent);

	  //Set inconsistent with everything else
	  const set<pair<int, int> > & inconsistent = inconsistent_facts[static_fluent.first][static_fluent.second];
	  for (set<pair<int, int> >::iterator it = inconsistent.begin();
	       it != inconsistent.end(); it++) {
	    if (!is_unreachable(it->first, it->second)) {
	      set_unreachable(it->first, it->second);
	      new_unreachable = true;
	    }
	  }
	}
      }
    }
  } while(new_unreachable);  

  for(int var = 0; var < unreachable.size(); var++){
    for(int val = 0; val < unreachable[var].size(); val++){
      if(variables[var]->is_reachable(val) && unreachable[var][val]){
	// cout << "Unreachable proposition: " << variables[var]->get_fact_name(val) << endl;
	variables[var]->set_unreachable(val);
      }
    }
  }

}


bool H2Mutexes::remove_spurious_operators(vector<Operator> &operators){
  int count = 0, totalCount = 0;
  bool spurious_detected = false;
  for (int i = 0; i < operators.size(); i++) {
    if (!operators[i].is_redundant()) {
      totalCount ++;
      operators[i].remove_ambiguity(*this);
      if(operators[i].is_redundant()) {
	spurious_detected = true;
	count ++;
      }
    }
  }
  cout << count  << " of " << totalCount << " operators detected as spurious" << endl;
  return spurious_detected;
}

bool H2Mutexes::initialize(const vector <Variable *> &variables,
			   const vector<MutexGroup> & mutexes){
  cout << "Initializing mutex computation..." << endl;
  num_vars = variables.size();
  num_vals.resize(num_vars);
  for(int i = 0; i < num_vars; i++){      
    num_vals[i] = variables[i]->get_range();
    //cout <<  variables[i]->get_name() << " i: " << i << " level: " << variables[i]->get_level() << " num_vals: " << num_vals[i] << endl;
  }

  number_props = 0;
  p_index.resize(variables.size());
  for (unsigned i = 0; i < variables.size(); i++){
    p_index[i].resize(variables[i]->get_range());
    for (unsigned j = 0; j < variables[i]->get_range(); j++){
      p_index_reverse.push_back(make_pair(i,j));
      p_index[i][j] = number_props++;
      //cout << i << " " << j << ": " << variables[i]->get_fact_name(j) << endl;
    }
  }

  unreachable.resize(num_vars);
  for(int i = 0; i < num_vars; i++){
    unreachable[i].resize(num_vals[i], false);
  }

  inconsistent_facts.resize(num_vars);
  for(int i = 0; i < num_vars; i++){
    inconsistent_facts[i].resize(num_vals[i]);
    //  cout << i << "-" << num_vals[i] << endl;
  }
  //Initialize everything to NOT_REACHED (mutexes will be set to spurious) 
  m_values.resize(number_props * number_props, NOT_REACHED);

  //Set to spurious variables with themselves
  for(int var = 0; var < num_vars; ++var){
    for(int val1 = 0; val1 < num_vals[var]; ++val1){
      int p_index_1 = p_index[var][val1];
      for(int val2 = val1 + 1; val2 < num_vals[var]; ++val2){
	int p_index_2 = p_index[var][val2];
	int pos1 = position(p_index_1, p_index_2);
	int pos2 = position(p_index_2, p_index_1);
	m_values[pos1] = SPURIOUS; 
	m_values[pos2] = SPURIOUS;
      }
    }
  }
    
  //cout << "Get mutexes already computed" << endl;
  for(int i = 0; i < mutexes.size(); i++){
    //cout << "Mutex: " << i << " of " << mutexes.size() << endl;
    vector<pair<int, int> > invariant_group;
    mutexes[i].get_invariant_group(invariant_group);
    for (size_t j = 0; j < invariant_group.size(); ++j) {
      const pair<int, int> &fact1 = invariant_group[j];
      int var1 = fact1.first, val1 = fact1.second;
      if(var1 == -1) continue;
      for (size_t k = 0; k < invariant_group.size(); ++k) {	 
	const pair<int, int> &fact2 = invariant_group[k];
	int var2 = fact2.first;
	if(var2 == -1) continue;
	int val2 = fact2.second; // Vidal: redundancy included
	if (var1 != var2) {
	  /* The "different variable" test makes sure we
	     don't mark a fact as mutex with itself
	     (important for correctness) and don't include
	     redundant mutexes (important to conserve
	     memory). Note that the preprocessor removes
	     mutex groups that contain *only* redundant
	     mutexes, but it can of course generate mutex
	     groups which lead to *some* redundant mutexes,
	     where some but not all facts talk about the
	     same variable. */
	  //cout << var1 << "-" << val1 << " is inconsistent with " << var2 << "-" << val2 << endl;
	  inconsistent_facts[var1][val1].insert(fact2);
	  inconsistent_facts[var2][val2].insert(fact1); // Vidal: redundancy included

	  // set the pairs that are mutex as spurious
	  m_values[position(p_index[var1][val1], p_index[var2][val2])] = SPURIOUS;
	  m_values[position(p_index[var2][val2], p_index[var1][val1])] = SPURIOUS;
	}
      }
    }
  }

  // We do not have any unreachable fluent at this point
  // // set the pairs with unreachable fluents as spurious
  // for (set<pair<int, int> >::iterator it = g_unreachable.begin(); it != g_unreachable.end(); ++it)
  //     for (unsigned i = 0; i < g_variable_domain.size(); i++)
  //         for (unsigned j = 0; j < g_variable_domain[i]; j++)
  //             m_values[position(p_index[i][j], p_index[it->first][it->second])] = m_values[position(p_index[it->first][it->second], p_index[i][j])] = SPURIOUS;

  cout << "Mutex computation initialized with " << number_props << " fluents." << endl;
  return true;
}


void H2Mutexes::init_values_progression(const vector <Variable *> &variables, const State & initial_state){
  int countSpurious = 0, countReached = 0, countNotReached = 0;
  
  for (unsigned i = 0; i < m_values.size(); i++){
    if(m_values[i] == SPURIOUS){
      countSpurious ++;
      continue;
    }
    m_values[i] = NOT_REACHED;
    countNotReached++;
  }

  for (unsigned i = 0; i < variables.size(); i++){
    int var1 = variables[i]->get_level();
    unsigned fluent1 = p_index[var1][initial_state[variables[i]]];
    for (unsigned j = 0; j < variables.size(); j++){
      int var2 = variables[j]->get_level();
      unsigned fluent2 = p_index[var2][initial_state[variables[j]]];
      unsigned pos = position(fluent1,fluent2); 
      //This check probably is unnecessary, because the initial state should not contain anything spurious
      // (I left it just in case of unsolvable problems)
      if(m_values[pos] == NOT_REACHED){
	m_values[pos] = REACHED;
	countReached++;
	countNotReached --;
      }
    }
  }
  cout << "Initialized mvalues forward: reached=" << countReached <<
    ", notReached=" << countNotReached << ", spurious=" << countSpurious << endl;

}


void H2Mutexes::init_values_regression(const vector<pair<Variable *, int> > & goal){
  cout << "Init values regression" << endl;
  for (unsigned i = 0; i < m_values.size(); i++){
    if(m_values[i] != SPURIOUS){
      m_values[i] = REACHED;
    }
  }
  
  // the things that are mutex with the goal are not reached
  for (unsigned g = 0; g < goal.size(); g++) {
    int gvar = goal[g].first->get_level();
    int gval = goal[g].second;
    //cout << "Goal: " << goal[g].first->get_fact_name(goal[g].second) << endl;
    const set<pair<int, int> > &goal_mutexes = inconsistent_facts[gvar][gval];
    for (set<pair<int, int> >::iterator it = goal_mutexes.begin(); it != goal_mutexes.end(); ++it) {
      setPropositionNotReached(p_index[it->first][it->second]);
    }
    for (int val1 = 0; val1 < num_vals[gvar]; val1++) {
      if(val1 != gval){
	setPropositionNotReached(p_index[gvar][val1]);
      }
    }
  }

  int countSpurious = 0, countReached = 0, countNotReached = 0;
  for (unsigned i = 0; i < m_values.size(); i++){
    if(m_values[i] == REACHED){
      countReached++;
    }else if (m_values[i] == NOT_REACHED){
      countNotReached++;
    }else{
      countSpurious++;
    }
  }


  cout << "Initialized mvalues backward: reached=" << countReached <<
    ", notReached=" << countNotReached << ", spurious=" << countSpurious << endl;
}

void H2Mutexes::setPropositionNotReached(int prop_index){
  for (unsigned var2 = 0; var2 < num_vars; var2++) {
    for (unsigned val2 = 0; val2 < num_vals[var2]; val2++) {
      int p_index_2 = p_index[var2][val2];
      int pos1 = position(prop_index, p_index_2);
      if (m_values[pos1] == REACHED) {
	m_values[pos1] = NOT_REACHED;
      }
      int pos2 = position(p_index_2, prop_index);
      if (m_values[pos2] == REACHED){
	m_values[pos2] = NOT_REACHED;
      }
    }
  }
}

void H2Mutexes::init_h2_operators(const vector<Operator> & operators, const vector<Axiom> & axioms, bool regression){
  m_ops.clear();
  m_ops.reserve(operators.size());
  for (unsigned i = 0; i < operators.size(); i++) {
    m_ops.push_back(Op_h2(operators[i], p_index, inconsistent_facts, regression));
  }

  //TODO: use axioms
  if(axioms.size()){
    cerr << "Error, axioms not supported by h2" << endl;
    exit(-1);
  }
}

//Returns the number of new mutexes or -1 if failed
int H2Mutexes::compute(const vector <Variable *> &variables,
		       vector<Operator> & operators, //operators is no t const because they may be detected as spurious
		       const vector<Axiom> & axioms,
		       const State &initial_state, 
		       const vector<pair<Variable *, int> > & goal,
                       vector<MutexGroup> & mutexes,
		       bool regression) {
  cout << "Initialize m_index " << (regression ? "bw" : "fw") << endl;
  if(regression){
    init_values_regression(goal);
  }else{
    init_values_progression(variables, initial_state);
  }
  cout << "Initialize m_ops " << (regression ? "bw" : "fw") << endl;
  init_h2_operators(operators, axioms, regression);

  cout << "Computing mutexes..." << endl;

  bool updated;
  do {
    if(time_exceeded())
      return -1;

    updated = false;
    for (unsigned op_i = 0; op_i < m_ops.size(); op_i++ ) {

      // disregard spurious operators
      if (m_ops[op_i].triggered == SPURIOUS)
	continue;

      // if the preconditions haven't been met, continue
      if ((m_ops[op_i].triggered != REACHED) &&
	  ( (m_ops[op_i].triggered = 
	     eval_propositions(m_ops[op_i].pre)) != REACHED))
	continue;

      
      for (unsigned add_i = 0; add_i < m_ops[op_i].add.size(); add_i++) {
	unsigned p = m_ops[op_i].add[add_i];
	for (unsigned add_j = 0; add_j < m_ops[op_i].add.size(); add_j++) {
	  unsigned q = m_ops[op_i].add[add_j];
	  if (m_values[position(p,q)] == NOT_REACHED) {
	    m_values[position(p,q)] = m_values[position(q,p)] = REACHED;
	    updated = true;
	  }
	}

		
	for (unsigned prop_i = 0; prop_i < number_props; prop_i++){
	  if (m_values[position(prop_i,prop_i)] != REACHED ||
	      m_values[position(p,prop_i)] != NOT_REACHED)
	    continue;

	  if (binary_search (m_ops[op_i].add.begin(),
			     m_ops[op_i].add.end(),
			     prop_i) ||
	      binary_search (m_ops[op_i].del.begin(),
			     m_ops[op_i].del.end(),
			     prop_i)){
	    continue;
	  }

	  bool satisfied = true;
	  for (unsigned pre_i = 0; satisfied && pre_i < m_ops[op_i].pre.size(); pre_i++){
	    satisfied = (m_values[position(prop_i,m_ops[op_i].pre[pre_i])] == REACHED);
	  }

	  if (satisfied) {
	    // pair<unsigned, unsigned> a = p_index_reverse[prop_i];
	    // pair<unsigned, unsigned> b = p_index_reverse[p];
	    // cout << "Action: " << g_operators[op_i].get_name() << " -> ";
	    // cout << print_fluent(a.first,a.second) << " - " << print_fluent(b.first,b.second) << endl;                     
	    m_values[position(p,prop_i)] = m_values[position(prop_i,p)] = REACHED;
	    updated = true;
	  }
	}
      }
    }
  } while (updated);

  int countReached = 0, countNotReached = 0, countSpurious = 0;
  for (unsigned i = 0; i < m_values.size(); i++){
    if(m_values[i] == REACHED){
      countReached++;
    }else if (m_values[i] == NOT_REACHED){
      countNotReached++;
    }else{
      countSpurious++;
    }
  }
    cout << "Mutex computation finished with reached=" << countReached <<
      ", notReached=" << countNotReached << ", spurious=" << countSpurious << endl;

    int numSpuriousOps = 0;
    for (unsigned op_i = 0; op_i < m_ops.size(); op_i++ ) {
      if(m_ops[op_i].triggered == NOT_REACHED){
	//cout << operators[op_i].get_name() << " is spurious because was not triggered" << endl;
	numSpuriousOps++;
	operators[op_i].set_spurious();
      }
    }
    cout << numSpuriousOps << " operators are spurious because were not triggered" << endl;

  //Add mutexes
  unsigned count = 0;
  for (unsigned i = 0; i < m_values.size(); i++){
    if (m_values[i] == NOT_REACHED){
      m_values[i] = SPURIOUS;
      pair<unsigned, unsigned> a = p_index_reverse[i / number_props];
      pair<unsigned, unsigned> b = p_index_reverse[i % number_props];
      if (a == b) {
	unreachable[a.first][a.second] = true;
      } else { 
	if(m_values[position(p_index[a.first][a.second], p_index[a.first][a.second])] == REACHED &&
	   m_values[position(p_index[b.first][b.second], p_index[b.first][b.second])] == REACHED){
	  // cout << "Mutex: " << variables[a.first]->get_fact_name(a.second) << " and " 
	  //      << variables[b.first]->get_fact_name(b.second) << endl;
	  //Only increase the mutex count when both fluents are reachable
	  count++;
          // add to mutex groups
          if (a.first < b.first) {
            vector <pair <int, int> > mut_group;
            mut_group.push_back(make_pair(a.first, a.second));
            mut_group.push_back(make_pair(b.first, b.second));
            mutexes.push_back(MutexGroup(mut_group, variables, regression));
          }
          // add to inconsistent
	  inconsistent_facts[a.first][a.second].insert(b);
	  inconsistent_facts[b.first][b.second].insert(a);
	}
      }
    }
  }

  int countUnreachable = 0;
  for(int var = 0; var < unreachable.size(); var++){
    for(int val = 0; val < unreachable[var].size(); val++){
      if(variables[var]->is_reachable(val) && unreachable[var][val]){
	// cout << "Unreachable proposition: " << variables[var]->get_fact_name(val) << endl;
	countUnreachable++;
	variables[var]->set_unreachable(val);
      }
    }
  }

  cout << "H^2 mutexes added " << (regression? "bw" :  "fw") << ": " << count << ", unreachable: " << countUnreachable << endl;
 
  return count + countUnreachable;
}

Reachability H2Mutexes::eval_propositions(vector<unsigned> props) {
  if (props.empty())
    return REACHED;
  for (unsigned i = 0; i < props.size(); i++)
    for (unsigned j = i; j < props.size(); j++)
      if (m_values[position(props[i],props[j])] == NOT_REACHED)
	return NOT_REACHED;
  return REACHED;
}

void H2Mutexes::print_mutexes() {
  unsigned count = 0;
  for (unsigned i = 0; i < m_values.size(); i++){
    if (m_values[i] == SPURIOUS){
      pair<unsigned, unsigned> a = p_index_reverse[i / number_props];
      pair<unsigned, unsigned> b = p_index_reverse[i % number_props];
      if (!are_mutex(a.first, a.second, b.first, b.second)) {
	count++;
	//cout << g_fact_names[a.first][a.second] << " related to " << g_fact_names[a.first][0] << " - " << g_fact_names[b.first][b.second] << " related to " << g_fact_names[b.first][0] << endl;
      }
    }
  }
  cout << count << " " << m_values.size() << endl;
}

void H2Mutexes::print_pair(unsigned /*pair*/) {
  //pair<unsigned, unsigned> a = p_index_reverse[pair / number_props];
  //pair<unsigned, unsigned> b = p_index_reverse[pair % number_props];
  //cout << g_fact_names[a.first][a.second] << " related to " << g_fact_names[a.first][0] << " - " << g_fact_names[b.first][b.second] << " related to " << g_fact_names[b.first][0] << endl;
}

bool H2Mutexes::time_exceeded(){
  if(limit_seconds == -1) // no limit
    return false;

  if(difftime(time(NULL),start) > limit_seconds){
    cout << "h^mutexes could not be computed (building time)" << endl;
    return true;
  }
  return false;
}


void Op_h2::instantiate_operator_forward(const Operator & op, const vector< vector<unsigned> > & p_index, 
					 const vector<vector<set<pair<int, int> > > > & inconsistent_facts) {
  vector<bool> prepost_var(inconsistent_facts.size(), false);

  const vector<Operator::Prevail> &prevail = op.get_prevail();
  for (unsigned j = 0; j < prevail.size(); j++)
    push_pre(p_index, prevail[j].var, prevail[j].prev);
  
  const vector<Operator::PrePost> &pre_post = op.get_pre_post();
  for (unsigned j = 0; j < pre_post.size(); j++) {
    if (pre_post[j].pre != -1) {
      push_pre(p_index, pre_post[j].var, pre_post[j].pre);
    }
    push_add(p_index, pre_post[j].var, pre_post[j].post);
    prepost_var[pre_post[j].var->get_level()] = true;
  }
  
  // fluents mutex with prevails are e-deleted: add as negative effect
  for (unsigned j = 0; j < prevail.size(); j++) {
    // pre.push_back(p_index[prevail[j].var][prevail[j].prev]);
    unsigned var = prevail[j].var->get_level();
    unsigned prev = prevail[j].prev;
    if(var == -1)
      continue;

    // fluents that belong to the same variable
    for (unsigned k = 0; k <  p_index[var].size(); k++)
      if (k != prev)
	del.push_back(p_index[var][k]);

    // fluents mutex with the prevail
    const set<pair<int, int> > prev_mutexes = inconsistent_facts[var][prev];
    for (set<pair<int, int> >::iterator it = prev_mutexes.begin(); it != prev_mutexes.end(); ++it)
      del.push_back(p_index[it->first][it->second]);
  }

  // fluents mutex with adds are e-deleted: add as negative effect
  for (unsigned j = 0; j < pre_post.size(); j++) {
    unsigned var = pre_post[j].var->get_level();
    unsigned post = pre_post[j].post;

    if (pre_post[j].is_conditional_effect){
      continue;
      // }else{
      // bool found = false;
      // for (unsigned k = 0; k < pre_post.size(); k++) {
      // 	//found = /*pre_post[k].var->get_level() == var && */pre_post[k].is_conditional_effect;
      // 	if(found) break;
      // }
      // if(found) continue;
    }

    if(var == -1) continue;

    // fluents that belong to the same variable
    for (unsigned k = 0; k < p_index[var].size(); k++){
      if (k != post){
    	del.push_back(p_index[var][k]);
      }
    }
      
    // fluents mutex with the add
    const set<pair<int, int> > prev_mutexes = inconsistent_facts[var][post];
    for (set<pair<int, int> >::iterator it = prev_mutexes.begin(); it != prev_mutexes.end(); ++it)
      del.push_back(p_index[it->first][it->second]);
  }

  // augmented preconditions from the disambiguation
  const vector<pair<int,int> > &augmented = op.get_augmented_preconditions();
  //     cout << "Augmented size: " << op.get_name() << " -> " << augmented.size() << endl;
  for (unsigned j = 0; j < augmented.size(); j++) {
    int var = augmented[j].first;
    int val = augmented[j].second;
    pre.push_back(p_index[var][val]);

    if (!prepost_var[var]) {
      // add the mutexes as deletes
      for (unsigned k = 0; k < p_index[var].size(); k++)
        if (k != augmented[j].second)
	  del.push_back(p_index[var][k]);
      const set<pair<int, int> > augmented_mutexes = inconsistent_facts[var][val];
      for (set<pair<int, int> >::iterator it = augmented_mutexes.begin(); it != augmented_mutexes.end(); ++it)
        del.push_back(p_index[it->first][it->second]);
    }
  }
}

void Op_h2::instantiate_operator_backward(const Operator & op,
					  const vector< vector<unsigned> > & p_index, 
					  const vector<vector<set<pair<int, int> > > > & inconsistent_facts) {
  vector<bool> prepost_var(inconsistent_facts.size(), false);

  const vector<Operator::Prevail> &prevail = op.get_prevail();
  for (unsigned j = 0; j < prevail.size(); j++)
    push_pre(p_index, prevail[j].var, prevail[j].prev);

  const vector<Operator::PrePost> &pre_post = op.get_pre_post();
  for (unsigned j = 0; j < pre_post.size(); j++) {
    if (pre_post[j].pre != -1) {
      push_add(p_index, pre_post[j].var, pre_post[j].pre);
    }
    push_pre(p_index, pre_post[j].var, pre_post[j].post);
    prepost_var[pre_post[j].var->get_level()] = true;

    if (pre_post[j].is_conditional_effect) {  // naive support for conditional effects
        vector<Operator::EffCond> effect_conds = pre_post[j].effect_conds;
        for (unsigned k = 0; k < effect_conds.size(); k++)
            push_add(p_index, effect_conds[k].var, effect_conds[k].cond);
    }

  }
        
  // fluents mutex with prevails are e-deleted: add as negative effect
  for (unsigned j = 0; j < prevail.size(); j++) {
    unsigned var = prevail[j].var->get_level();
    unsigned prev = prevail[j].prev;

    if(var == -1) continue;
    
    // fluents that belong to the same variable
    for (unsigned k = 0; k < p_index[var].size(); k++)
      if (k != prev)
	del.push_back(p_index[var][k]);
    
    // fluents mutex with the prevail
    const set<pair<int, int> > prev_mutexes = inconsistent_facts[var][prev];
    for (set<pair<int, int> >::iterator it = prev_mutexes.begin(); it != prev_mutexes.end(); ++it)
      del.push_back(p_index[it->first][it->second]);
  }

  // fluents mutex with pres are e-deleted: add as negative effect
  for (unsigned j = 0; j < pre_post.size(); j++) {
    if (pre_post[j].is_conditional_effect)
      continue;
    // pre.push_back(p_index[prevail[j].var][prevail[j].prev]);
    unsigned var = pre_post[j].var->get_level();
    unsigned pre = pre_post[j].pre;
    if (var == -1 || pre == -1)
      continue;

    // fluents that belong to the same variable
    for (unsigned k = 0; k < p_index[var].size(); k++)
      if (k != pre)
	del.push_back(p_index[var][k]);

    // fluents mutex with the add
    const set<pair<int, int> > pre_mutexes = inconsistent_facts[var][pre];
    for (set<pair<int, int> >::iterator it = pre_mutexes.begin(); it != pre_mutexes.end(); ++it)
      del.push_back(p_index[it->first][it->second]);
  }

  // augmented preconditions from the disambiguation
  const vector<pair<int,int> > &augmented = op.get_augmented_preconditions();
  //     cout << "Augmented size: " << op.get_name() << " -> " << augmented.size() << endl;
  for (unsigned j = 0; j < augmented.size(); j++) {
    int var = augmented[j].first;
    int val = augmented[j].second;
    // add the precondition as an add
    //         cout << "Augmented: " << op.get_name() << " -> " << print_fluent(augmented[j].first, augmented[j].second) << endl;
    if (!prepost_var[var])
        pre.push_back(p_index[var][val]);

    // add the mutexes as deletes
    for (unsigned k = 0; k < p_index[var].size(); k++)
      if (k != augmented[j].second)
	del.push_back(p_index[var][k]);
    const set<pair<int, int> > augmented_mutexes = inconsistent_facts[var][val];
    for (set<pair<int, int> >::iterator it = augmented_mutexes.begin(); it != augmented_mutexes.end(); ++it)
      del.push_back(p_index[it->first][it->second]);
  }

  // potential preconditions from the disambiguation
  const vector<pair<int,int> > &potential = op.get_potential_preconditions();
  for (unsigned j = 0; j < potential.size(); j++) {
    // add the precondition as an add
    add.push_back(p_index[potential[j].first][potential[j].second]);
    // add the mutexes as deletes
    for (unsigned k = 0; k < p_index[potential[j].first].size(); k++)
      if (k != potential[j].second)
	del.push_back(p_index[potential[j].first][potential[j].second]);
    const set<pair<int, int> > potential_mutexes = inconsistent_facts[potential[j].first][potential[j].second];
    for (set<pair<int, int> >::iterator it = potential_mutexes.begin(); it != potential_mutexes.end(); ++it)
      del.push_back(p_index[it->first][it->second]);
  }
}
