#include "symbolic_abstraction.h"

#include "label_reducer.h"
#include "merge_and_shrink_heuristic.h" // needed for ShrinkStrategy type;
// TODO: move that type somewhere else?
#include "shrink_fh.h"
#include "symbolic_exploration/sym_exploration.h"

#include "../globals.h"
#include "../operator.h"
#include "../option_parser.h" // TODO: Should be removable later.
#include "../priority_queue.h"
#include "../timer.h"

#include <algorithm>
#include <cassert>
#include <cstdlib>
#include <cstring>
#include <deque>
#include <iostream>
#include <limits>
#include <string>
#include <sstream>
using namespace std;

//  TODO: We define infinity in more than a few places right now (=>
//        grep for it). It should only be defined once.
static const int infinity = numeric_limits<int>::max();

inline int get_op_index(const Operator *op) {
    /* TODO: The op_index computation is duplicated from
     LabelReducer::get_op_index() and actually belongs neither
     here nor there. There should be some canonical way of getting
     from an Operator pointer to an index, but it's not clear how to
     do this in a way that best fits the overall planner
     architecture (taking into account axioms etc.) */
    int op_index = op - &*g_operators.begin();
    assert(op_index >= 0 && op_index < g_operators.size());
    return op_index;
}


SymbolicAbstraction::SymbolicAbstraction(SymManager * mgr,
					 bool is_unit_cost_,
					 OperatorCost cost_type_, int variable) :   manager(mgr),  is_unit_cost(is_unit_cost_), cost_type(cost_type_), exploration(NULL), existAbsStateVars(mgr->oneBDD()), biimpAbsStateVars(mgr->oneBDD()),are_labels_reduced(false), peak_memory(0) {


  clear_distances();
  transitions_by_op.resize(g_operators.size());

  varset.push_back(variable);

  biimpAbsStateVars = mgr->getVars().biimp(variable);
  vector<int> varsS = mgr->varsS(variable);
  for(int i = 0; i < varsS.size(); i++){
    absVarsS.push_back(mgr->getVars().bddVar(varsS[i]));
    absVarsSp.push_back(mgr->getVars().bddVar(varsS[i] + 1));
    existAbsStateVars *= absVarsS.back();
  }

  /*
    This generates the states of the atomic abstraction, but not the
    arcs: It is more efficient to generate all arcs of all atomic
    abstractions simultaneously.
  */
    int range = g_variable_domain[variable];

    int init_value = (*g_initial_state)[variable];
    int goal_value = -1;
    for (int goal_no = 0; goal_no < g_goal.size(); goal_no++) {
        if (g_goal[goal_no].first == variable) {
            assert(goal_value == -1);
            goal_value = g_goal[goal_no].second;
        }
    }

    num_states = range;
    goal_states.resize(num_states, false);
    bdds.reserve(num_states);
    for (int value = 0; value < range; value++) {
        if (value == goal_value || goal_value == -1) {
            goal_states[value] = true;
        }
        if (value == init_value)
            init_state = value;
	bdds.push_back(manager->getBDD(variable, value));
    }

}

SymbolicAbstraction::SymbolicAbstraction(SymManager * mgr,
					 bool is_unit_cost_,
					 OperatorCost cost_type_,
					 SymbolicAbstraction *abs1,
					 SymbolicAbstraction *abs2): 
  manager(mgr), is_unit_cost(is_unit_cost_), cost_type(cost_type_),
  exploration (abs1->exploration), 
  are_labels_reduced(false), peak_memory(0) {

  clear_distances();
  transitions_by_op.resize(g_operators.size());

  cout << "Merging " << abs1->tag() << " and "
         << abs2->tag() << endl;

    assert(abs1->is_solvable() && abs2->is_solvable());


    ::set_union(abs1->varset.begin(), abs1->varset.end(), abs2->varset.begin(),
                abs2->varset.end(), back_inserter(varset));

    for(int i = 0; i < abs1->absVarsS.size(); i++){
      absVarsS.push_back(abs1->absVarsS[i]);
    }
    for(int i = 0; i < abs2->absVarsS.size(); i++){
      absVarsS.push_back(abs2->absVarsS[i]);
    }

    for(int i = 0; i < abs1->absVarsSp.size(); i++){
      absVarsSp.push_back(abs1->absVarsSp[i]);
    }
    for(int i = 0; i < abs2->absVarsSp.size(); i++){
      absVarsSp.push_back(abs2->absVarsSp[i]);
    }    
    
    existAbsStateVars = abs1->existAbsStateVars*abs2->existAbsStateVars;
    biimpAbsStateVars = abs1->biimpAbsStateVars*abs2->biimpAbsStateVars;



    num_states = abs1->size() * abs2->size();
    goal_states.resize(num_states, false);
    bdds.resize(num_states);

    for (int s1 = 0; s1 < abs1->size(); s1++) {
        for (int s2 = 0; s2 < abs2->size(); s2++) {
            int state = s1 * abs2->size() + s2;
	    bdds[state] = abs1->bdds[s1] * abs2->bdds[s2];
	  
            if (abs1->goal_states[s1] && abs2->goal_states[s2])
                goal_states[state] = true;
            if (s1 == abs1->init_state && s2 == abs2->init_state)
                init_state = state;
        }
    }

    for (int i = 0; i < abs1->relevant_operators.size(); i++)
        abs1->relevant_operators[i]->marker1 = true;
    for (int i = 0; i < abs2->relevant_operators.size(); i++)
        abs2->relevant_operators[i]->marker2 = true;

    int multiplier = abs2->size();
    for (int op_no = 0; op_no < g_operators.size(); op_no++) {
        const Operator *op = &g_operators[op_no];
	if(op->spurious) continue;
        bool relevant1 = op->marker1;
        bool relevant2 = op->marker2;
        if (relevant1 || relevant2) {
            vector<AbstractTransition> &transitions = transitions_by_op[op_no];
            relevant_operators.push_back(op);
            const vector<AbstractTransition> &bucket1 =
                abs1->transitions_by_op[op_no];
            const vector<AbstractTransition> &bucket2 =
                abs2->transitions_by_op[op_no];
            if (relevant1 && relevant2) {
                transitions.reserve(bucket1.size() * bucket2.size());
                for (int i = 0; i < bucket1.size(); i++) {
                    int src1 = bucket1[i].src;
                    int target1 = bucket1[i].target;
                    for (int j = 0; j < bucket2.size(); j++) {
                        int src2 = bucket2[j].src;
                        int target2 = bucket2[j].target;
                        int src = src1 * multiplier + src2;
                        int target = target1 * multiplier + target2;
                        transitions.push_back(AbstractTransition(src, target));
                    }
                }
            } else if (relevant1) {
                assert(!relevant2);
                transitions.reserve(bucket1.size() * abs2->size());
                for (int i = 0; i < bucket1.size(); i++) {
                    int src1 = bucket1[i].src;
                    int target1 = bucket1[i].target;
                    for (int s2 = 0; s2 < abs2->size(); s2++) {
                        int src = src1 * multiplier + s2;
                        int target = target1 * multiplier + s2;
                        transitions.push_back(AbstractTransition(src, target));
                    }
                }
            } else if (relevant2) {
                assert(!relevant1);
                transitions.reserve(bucket2.size() * abs1->size());
                for (int i = 0; i < bucket2.size(); i++) {
                    int src2 = bucket2[i].src;
                    int target2 = bucket2[i].target;
                    for (int s1 = 0; s1 < abs1->size(); s1++) {
                        int src = s1 * multiplier + src2;
                        int target = s1 * multiplier + target2;
                        transitions.push_back(AbstractTransition(src, target));
                    }
                }
            }
	}else{
	  //Not relevant
	  irrelevant_operators.push_back(op);
	  /*	  cout << "Abstraction " << tag() << " has irrelevant op: ";
		  op->dump();*/

	}
    }

    for (int i = 0; i < abs1->relevant_operators.size(); i++)
        abs1->relevant_operators[i]->marker1 = false;
    for (int i = 0; i < abs2->relevant_operators.size(); i++)
        abs2->relevant_operators[i]->marker2 = false;
}


SymbolicAbstraction::~SymbolicAbstraction(){}

string SymbolicAbstraction::tag() const {
  std::stringstream result;
  result << "SymbolicAbstraction [";
  /*std::copy(varset.begin(), varset.end()-1, 
    std::ostream_iterator<int>(result, " "));*/
  result << *varset.begin() << "-" << varset.back() << "(" << varset.size() << ")] ";
  return result.str();
}




void SymbolicAbstraction::clear_distances() {
    max_f = DISTANCE_UNKNOWN;
    max_g = DISTANCE_UNKNOWN;
    max_h = DISTANCE_UNKNOWN;
    init_distances.clear();
    goal_distances.clear();
}

int SymbolicAbstraction::size() const {
    return num_states;
}

int SymbolicAbstraction::get_max_f() const {
    return max_f;
}

int SymbolicAbstraction::get_max_g() const {
    return max_g;
}

int SymbolicAbstraction::get_max_h() const {
    return max_h;
}

void SymbolicAbstraction::compute_distances() {
    cout << tag() << flush;
    if (max_h != DISTANCE_UNKNOWN) {
      cout << "distances already known, max_h=" << max_h <<  endl;
        return;
    }

    assert(init_distances.empty() && goal_distances.empty());

    if (init_state == PRUNED_STATE) {
        cout << "init state was pruned, no distances to compute" << endl;
        // If init_state was pruned, then everything must have been pruned.
        assert(num_states == 0);
        max_f = max_g = max_h = infinity;
        return;
    }

    init_distances.resize(num_states, infinity);
    goal_distances.resize(num_states, infinity);
    if (is_unit_cost) {
        cout << "computing distances using unit-cost algorithm" << endl;
        compute_init_distances_unit_cost();
        compute_goal_distances_unit_cost();
    } else {
        cout << "computing distances using general-cost algorithm" << endl;
        compute_init_distances_general_cost();
        compute_goal_distances_general_cost();
    }


    max_f = 0;
    max_g = 0;
    max_h = 0;

    int unreachable_count = 0, irrelevant_count = 0;
    for (int i = 0; i < num_states; i++) {
        int g = init_distances[i];
        int h = goal_distances[i];
        // States that are both unreachable and irrelevant are counted
        // as unreachable, not irrelevant. (Doesn't really matter, of
        // course.)
        if (g == infinity) {
            unreachable_count++;
        } else if (h == infinity) {
            irrelevant_count++;
        } else {
            max_f = max(max_f, g + h);
            max_g = max(max_g, g);
            max_h = max(max_h, h);
        }
    }

    if (unreachable_count || irrelevant_count) {
        cout << tag()
             << "unreachable: " << unreachable_count << " states, "
             << "irrelevant: " << irrelevant_count << " states" << endl;
        /* Call shrink to discard unreachable and irrelevant states.
           The strategy must be one that prunes unreachable/irrelevant
           notes, but beyond that the details don't matter, as there
           is no need to actually shrink. So faster methods should be
           preferred. */

        /* TODO: Create a dedicated shrinking strategy from scratch,
           e.g. a bucket-based one that simply generates one good and
           one bad bucket? */

        // TODO/HACK: The way this is created is of course unspeakably
        // ugly. We'll leave this as is for now because there will likely
        // be more structural changes soon.
        ShrinkStrategy *shrink_temp = ShrinkFH::create_default(num_states);
        shrink_temp->shrink(*this, num_states, true);
        delete shrink_temp;
    }

}

int SymbolicAbstraction::get_cost_for_op(int op_no) const {
    return get_adjusted_action_cost(g_operators[op_no], cost_type);
}

static void breadth_first_search(
    const vector<vector<int> > &graph, deque<int> &queue,
    vector<int> &distances) {
    while (!queue.empty()) {
        int state = queue.front();
        queue.pop_front();
        for (int i = 0; i < graph[state].size(); i++) {
            int successor = graph[state][i];
            if (distances[successor] > distances[state] + 1) {
                distances[successor] = distances[state] + 1;
                queue.push_back(successor);
            }
        }
    }
}

void SymbolicAbstraction::compute_init_distances_unit_cost() {
    vector<vector<AbstractStateRef> > forward_graph(num_states);
    for (int i = 0; i < transitions_by_op.size(); i++) {
        const vector<AbstractTransition> &transitions = transitions_by_op[i];
        for (int j = 0; j < transitions.size(); j++) {
            const AbstractTransition &trans = transitions[j];
            forward_graph[trans.src].push_back(trans.target);
        }
    }

    deque<AbstractStateRef> queue;
    for (AbstractStateRef state = 0; state < num_states; state++) {
        if (state == init_state) {
            init_distances[state] = 0;
            queue.push_back(state);
        }
    }
    breadth_first_search(forward_graph, queue, init_distances);
}

void SymbolicAbstraction::compute_goal_distances_unit_cost() {
    vector<vector<AbstractStateRef> > backward_graph(num_states);
    for (int i = 0; i < transitions_by_op.size(); i++) {
        const vector<AbstractTransition> &transitions = transitions_by_op[i];
        for (int j = 0; j < transitions.size(); j++) {
            const AbstractTransition &trans = transitions[j];
            backward_graph[trans.target].push_back(trans.src);
        }
    }

    deque<AbstractStateRef> queue;
    for (AbstractStateRef state = 0; state < num_states; state++) {
        if (goal_states[state]) {
            goal_distances[state] = 0;
            queue.push_back(state);
        }
    }
    breadth_first_search(backward_graph, queue, goal_distances);
}

static void dijkstra_search(
    const vector<vector<pair<int, int> > > &graph,
    AdaptiveQueue<int> &queue,
    vector<int> &distances) {
    while (!queue.empty()) {
        pair<int, int> top_pair = queue.pop();
        int distance = top_pair.first;
        int state = top_pair.second;
        int state_distance = distances[state];
        assert(state_distance <= distance);
        if (state_distance < distance)
            continue;
        for (int i = 0; i < graph[state].size(); i++) {
            const pair<int, int> &transition = graph[state][i];
            int successor = transition.first;
            int cost = transition.second;
            int successor_cost = state_distance + cost;
            if (distances[successor] > successor_cost) {
                distances[successor] = successor_cost;
                queue.push(successor_cost, successor);
            }
        }
    }
}

void SymbolicAbstraction::compute_init_distances_general_cost() {
    vector<vector<pair<int, int> > > forward_graph(num_states);
    for (int i = 0; i < transitions_by_op.size(); i++) {
        int op_cost = get_cost_for_op(i);
        const vector<AbstractTransition> &transitions = transitions_by_op[i];
        for (int j = 0; j < transitions.size(); j++) {
            const AbstractTransition &trans = transitions[j];
            forward_graph[trans.src].push_back(
                make_pair(trans.target, op_cost));
        }
    }

    // TODO: Reuse the same queue for multiple computations to save speed?
    //       Also see compute_goal_distances_general_cost.
    AdaptiveQueue<int> queue;
    for (AbstractStateRef state = 0; state < num_states; state++) {
        if (state == init_state) {
            init_distances[state] = 0;
            queue.push(0, state);
        }
    }
    dijkstra_search(forward_graph, queue, init_distances);
}

void SymbolicAbstraction::compute_goal_distances_general_cost() {
    vector<vector<pair<int, int> > > backward_graph(num_states);
    for (int i = 0; i < transitions_by_op.size(); i++) {
        int op_cost = get_cost_for_op(i);
        const vector<AbstractTransition> &transitions = transitions_by_op[i];
        for (int j = 0; j < transitions.size(); j++) {
            const AbstractTransition &trans = transitions[j];
            backward_graph[trans.target].push_back(
                make_pair(trans.src, op_cost));
        }
    }

    // TODO: Reuse the same queue for multiple computations to save speed?
    //       Also see compute_init_distances_general_cost.
    AdaptiveQueue<int> queue;
    for (AbstractStateRef state = 0; state < num_states; state++) {
        if (goal_states[state]) {
            goal_distances[state] = 0;
            queue.push(0, state);
        }
    }
    dijkstra_search(backward_graph, queue, goal_distances);
}



void SymbolicAbstraction::normalize(bool reduce_labels) {
    // Apply label reduction and remove duplicate transitions.

    // dump();

    cout << tag() << "normalizing ";

    LabelReducer *reducer = 0;
    if (reduce_labels) {
        if (are_labels_reduced) {
            cout << "without label reduction (already reduced)" << endl;
        } else {
            cout << "with label reduction" << endl;
            reducer = new LabelReducer(relevant_operators, varset, cost_type);
            reducer->statistics();
            are_labels_reduced = true;
        }
    } else {
        cout << "without label reduction" << endl;
    }

    typedef vector<pair<AbstractStateRef, int> > StateBucket;

    /* First, partition by target state. Also replace operators by
       their canonical representatives via label reduction and clear
       away the transitions that have been processed. */
    vector<StateBucket> target_buckets(num_states);
    for (int op_no = 0; op_no < transitions_by_op.size(); op_no++) {
        vector<AbstractTransition> &transitions = transitions_by_op[op_no];
        if (!transitions.empty()) {
            int reduced_op_no;
            if (reducer) {
                const Operator *op = &g_operators[op_no];
                const Operator *reduced_op = reducer->get_reduced_label(op);
                reduced_op_no = get_op_index(reduced_op);
            } else {
                reduced_op_no = op_no;
            }
            for (int i = 0; i < transitions.size(); i++) {
                const AbstractTransition &t = transitions[i];
                target_buckets[t.target].push_back(
                    make_pair(t.src, reduced_op_no));
            }
            vector<AbstractTransition> ().swap(transitions);
        }
    }

    // Second, partition by src state.
    vector<StateBucket> src_buckets(num_states);

    for (AbstractStateRef target = 0; target < num_states; target++) {
        StateBucket &bucket = target_buckets[target];
        for (int i = 0; i < bucket.size(); i++) {
            AbstractStateRef src = bucket[i].first;
            int op_no = bucket[i].second;
            src_buckets[src].push_back(make_pair(target, op_no));
        }
    }
    vector<StateBucket> ().swap(target_buckets);

    // Finally, partition by operator and drop duplicates.
    for (AbstractStateRef src = 0; src < num_states; src++) {
        StateBucket &bucket = src_buckets[src];
        for (int i = 0; i < bucket.size(); i++) {
            int target = bucket[i].first;
            int op_no = bucket[i].second;

            vector<AbstractTransition> &op_bucket = transitions_by_op[op_no];
            AbstractTransition trans(src, target);
            if (op_bucket.empty() || op_bucket.back() != trans)
                op_bucket.push_back(trans);
        }
    }

    delete reducer;
    // dump();
}



void SymbolicAbstraction::build_atomic_abstractions(SymManager * mgr,
    bool is_unit_cost_, OperatorCost cost_type_,
    vector<SymbolicAbstraction *> &result) {
    assert(result.empty());
    cout << "Building atomic abstractions... " << endl;
    int var_count = g_variable_domain.size();

    // Step 1: Create the abstraction objects without transitions.
    for (int var_no = 0; var_no < var_count; var_no++)
      result.push_back(new SymbolicAbstraction(mgr,
                             is_unit_cost_, cost_type_, var_no));

    // Step 2: Add transitions.
    for (int op_no = 0; op_no < g_operators.size(); op_no++) {
        const Operator *op = &g_operators[op_no];
	if(op->spurious){
	  cout << "Skip " << op->get_name() << endl;
	  continue;
	}
        const vector<Prevail> &prev = op->get_prevail();
        for (int i = 0; i < prev.size(); i++) {
            int var = prev[i].var;
            int value = prev[i].prev;
            SymbolicAbstraction *abs = result[var];
            AbstractTransition trans(value, value);
            abs->transitions_by_op[op_no].push_back(trans);

            if (abs->relevant_operators.empty()
                || abs->relevant_operators.back() != op)
                abs->relevant_operators.push_back(op);
        }
        const vector<PrePost> &pre_post = op->get_pre_post();
        for (int i = 0; i < pre_post.size(); i++) {
            int var = pre_post[i].var;
            int pre_value = pre_post[i].pre;
            int post_value = pre_post[i].post;
            SymbolicAbstraction *abs = result[var];
            int pre_value_min, pre_value_max;
            if (pre_value == -1) {
                pre_value_min = 0;
                pre_value_max = g_variable_domain[var];
            } else {
                pre_value_min = pre_value;
                pre_value_max = pre_value + 1;
            }
            for (int value = pre_value_min; value < pre_value_max; value++) {
                AbstractTransition trans(value, post_value);
                abs->transitions_by_op[op_no].push_back(trans);
            }
            if (abs->relevant_operators.empty()
                || abs->relevant_operators.back() != op)
                abs->relevant_operators.push_back(op);
        }
	for(int var = 0; var < result.size(); var++){
	  SymbolicAbstraction *abs = result[var];
	  if (abs->relevant_operators.empty()
	      || abs->relevant_operators.back() != op)
	    abs->irrelevant_operators.push_back(op);
	}
	 
    }
}




void SymbolicAbstraction::apply_abstraction(
    vector<slist<AbstractStateRef> > &collapsed_groups) {
    /* Note on how this method interacts with the distance information
       (init_distances and goal_distances): if no two states with
       different g or h values are combined by the abstraction (i.e.,
       if the abstraction is "f-preserving", then this method makes
       sure sure that distance information is preserved.

       This is important because one of the (indirect) callers of this
       method is the distance computation code, which uses it in a
       somewhat roundabout way to get rid of irrelevant and
       unreachable states. That caller will always give us an
       f-preserving abstraction.

       When called with a non-f-preserving abstraction, distance
       information is cleared as a side effect. In most cases we won't
       actually need it any more at this point anyway, so it is no
       great loss.

       Still, it might be good if we could find a way to perform the
       unreachability and relevance pruning that didn't introduce such
       tight coupling between the distance computation and abstraction
       code. It would probably also a good idea to do the
       unreachability/relevance pruning as early as possible, e.g.
       right after construction.
     */

    cout << tag() << "applying abstraction (" << size()
         << " to " << collapsed_groups.size() << " states)" << endl;

    /*    BDD allBDDs = bdds[0];
    for(int i = 0; i < bdds.size();i++){
      allBDDs += bdds[i];
      }*/

    //    cout << "Group 0 has " << collapsed_groups[0].size() << " nodes" << endl;

    //    cout << "All BDDs: "; allBDDs.print(1, 2);

    typedef slist<AbstractStateRef> Group;

    vector<int> abstraction_mapping(num_states, PRUNED_STATE);

    for (int group_no = 0; group_no < collapsed_groups.size(); group_no++) {
        Group &group = collapsed_groups[group_no];
	//	cout << "Looking at group: " << group.size() << endl;
        for (Group::iterator pos = group.begin(); pos != group.end(); ++pos) {
            AbstractStateRef state = *pos;
            assert(abstraction_mapping[state] == PRUNED_STATE);
            abstraction_mapping[state] = group_no;
        }
    }
    
    int new_num_states = collapsed_groups.size();
    vector<int> new_init_distances(new_num_states, infinity);
    vector<int> new_goal_distances(new_num_states, infinity);
    vector<bool> new_goal_states(new_num_states, false);
    vector<BDD> new_bdds(new_num_states);

    bool must_clear_distances = false;
    for (AbstractStateRef new_state = 0;
	 new_state < collapsed_groups.size(); new_state++) {
        Group &group = collapsed_groups[new_state];
        assert(!group.empty());
        Group::iterator pos = group.begin();
        int &new_init_dist = new_init_distances[new_state];
        int &new_goal_dist = new_goal_distances[new_state];

        new_init_dist = init_distances[*pos];
        new_goal_dist = goal_distances[*pos];

        new_goal_states[new_state] = goal_states[*pos];


	new_bdds[new_state] = bdds[*pos];


        ++pos;
        for (; pos != group.end(); ++pos) {
            if (init_distances[*pos] < new_init_dist) {
                must_clear_distances = true;
                new_init_dist = init_distances[*pos];
            }
            if (goal_distances[*pos] < new_goal_dist) {
                must_clear_distances = true;
                new_goal_dist = goal_distances[*pos];
            }
            if (goal_states[*pos])
                new_goal_states[new_state] = true;

	    new_bdds[new_state] += bdds[*pos];
        }

    }


    // Release memory.
    vector<int>().swap(init_distances);
    vector<int>().swap(goal_distances);
    vector<bool>().swap(goal_states);

    vector<vector<AbstractTransition> > new_transitions_by_op(
        transitions_by_op.size());
    for (int op_no = 0; op_no < transitions_by_op.size(); op_no++) {
        const vector<AbstractTransition> &transitions =
            transitions_by_op[op_no];
        vector<AbstractTransition> &new_transitions =
            new_transitions_by_op[op_no];
        new_transitions.reserve(transitions.size());
        for (int i = 0; i < transitions.size(); i++) {
            const AbstractTransition &trans = transitions[i];
            int src = abstraction_mapping[trans.src];
            int target = abstraction_mapping[trans.target];
            if (src != PRUNED_STATE && target != PRUNED_STATE)
                new_transitions.push_back(AbstractTransition(src, target));
        }
    }
    vector<vector<AbstractTransition> > ().swap(transitions_by_op);

    num_states = new_num_states;
    transitions_by_op.swap(new_transitions_by_op);
    init_distances.swap(new_init_distances);
    goal_distances.swap(new_goal_distances);
    goal_states.swap(new_goal_states);
    init_state = abstraction_mapping[init_state];
    if (init_state == PRUNED_STATE)
        cout << tag() << "initial state pruned; task unsolvable" << endl;

    //    apply_abstraction_to_lookup_table(abstraction_mapping); 
    //    cout << "Shrink exploration" << endl;
    if(exploration){
      exploration->shrink(bdds, new_bdds, collapsed_groups, existAbsStateVars);
    }
    
    //    cout << "Done shrink" << endl;
    
    vector<BDD>().swap(bdds);
    bdds.swap(new_bdds);
    if (must_clear_distances) {
        cout << tag() << "simplification was not f-preserving!" << endl;
        clear_distances();
    }    
    //    printBDDs();
}



bool SymbolicAbstraction::is_solvable() const {
    return init_state != PRUNED_STATE;
}

int SymbolicAbstraction::memory_estimate() const {
    int result = sizeof(SymbolicAbstraction);
    result += sizeof(Operator *) * relevant_operators.capacity();
    result += sizeof(Operator *) * irrelevant_operators.capacity();
    result += sizeof(vector<AbstractTransition> )
              * transitions_by_op.capacity();
    for (int i = 0; i < transitions_by_op.size(); i++)
        result += sizeof(AbstractTransition) * transitions_by_op[i].capacity();
    result += sizeof(int) * init_distances.capacity();
    result += sizeof(int) * goal_distances.capacity();
    result += sizeof(bool) * goal_states.capacity();
    //lalala    result += bddsSize;
    return result;
}

void SymbolicAbstraction::release_memory() {
    vector<const Operator *>().swap(relevant_operators);
    vector<const Operator *>().swap(irrelevant_operators);
    vector<vector<AbstractTransition> >().swap(transitions_by_op);
}

int SymbolicAbstraction::total_transitions() const {
    int total = 0;
    for (int i = 0; i < transitions_by_op.size(); i++)
        total += transitions_by_op[i].size();
    return total;
}

int SymbolicAbstraction::unique_unlabeled_transitions() const {
    vector<AbstractTransition> unique_transitions;
    for (int i = 0; i < transitions_by_op.size(); i++) {
        const vector<AbstractTransition> &trans = transitions_by_op[i];
        unique_transitions.insert(unique_transitions.end(), trans.begin(),
                                  trans.end());
    }
    ::sort(unique_transitions.begin(), unique_transitions.end());
    return unique(unique_transitions.begin(), unique_transitions.end())
           - unique_transitions.begin();
}

void SymbolicAbstraction::statistics(bool include_expensive_statistics) const {
    int memory = memory_estimate();
    peak_memory = max(peak_memory, memory);
    cout << tag() << size() << " states, ";
    if (include_expensive_statistics)
        cout << unique_unlabeled_transitions();
    else
        cout << "???";
    cout << "/" << total_transitions() << " arcs, " << memory << " bytes"
         << endl;
    cout << tag();
    if (max_h == DISTANCE_UNKNOWN) {
        cout << "distances not computed";
    } else if (is_solvable()) {
        cout << "init h=" << goal_distances[init_state] << ", max f=" << max_f
             << ", max g=" << max_g << ", max h=" << max_h;
    } else {
        cout << "abstraction is unsolvable";
    }
    cout << " [t=" << g_timer << "]" << endl;
}

int SymbolicAbstraction::get_peak_memory_estimate() const {
    return peak_memory;
}

bool SymbolicAbstraction::is_in_varset(int var) const {
    return find(varset.begin(), varset.end(), var) != varset.end();
}

void SymbolicAbstraction::dump() const {
    cout << "digraph abstract_transition_graph";
    for (int i = 0; i < varset.size(); i++)
        cout << "_" << varset[i];
    cout << " {" << endl;
    cout << "    node [shape = none] start;" << endl;
    for (int i = 0; i < num_states; i++) {
        bool is_init = (i == init_state);
        bool is_goal = (goal_states[i] == true);
        cout << "    node [shape = " << (is_goal ? "doublecircle" : "circle")
             << "] node" << i << ";" << endl;
        if (is_init)
            cout << "    start -> node" << i << ";" << endl;
    }
    assert(transitions_by_op.size() == g_operators.size());
    for (int op_no = 0; op_no < g_operators.size(); op_no++) {
        const vector<AbstractTransition> &trans = transitions_by_op[op_no];
        for (int i = 0; i < trans.size(); i++) {
            int src = trans[i].src;
            int target = trans[i].target;
            cout << "    node" << src << " -> node" << target << " [label = o_"
                 << op_no << "];" << endl;
        }
    }
    cout << "}" << endl;
}


ADD SymbolicAbstraction::getHeuristicADD(){
  compute_distances();
  ADD h = manager->mgr().constant(-1);
  for(int i = 0; i < num_states; i++){
    /*cout << "Adding " << i << " bdd " << bdds[i] <<
      " goal distance " << goal_distances[i] << 
      "nodes: " << bdds[i].nodeCount() << endl;*/
    //ADD aux1 = bdds[i].Add();
    //ADD aux2 = manager->mgr().constant(goal_distances[i]);
    //ADD aux = aux1*aux2;
    //cout << "Aux size: " << aux1.nodeCount() << " " << aux2.nodeCount() << " " << aux.nodeCount() << endl;
    //h += aux;
    h += bdds[i].Add()*manager->mgr().constant(goal_distances[i] +1);
  }
  //cout << "Not reached: " << not_reached << "nodes: " << not_reached.nodeCount() << endl;
  
  //ADD aux1 = not_reached.Add();
  //ADD aux2 = manager->mgr().constant(-1);
  //ADD aux = aux1*aux2;
  //cout << "Aux size: " << aux1.nodeCount() << " " << aux2.nodeCount() << " " << aux.nodeCount() << endl;
  //h += aux;
  cout << "ADD Heuristic size: " << h.nodeCount() << endl;
  return h;
}

void SymbolicAbstraction::getHeuristicBDD(hash_map<int, BDD> & res){
  compute_distances();
  BDD not_reached = manager->oneBDD();

  for(int i = 0; i < num_states; i++){
    int h =   goal_distances[i];
    if(h == PRUNED_STATE) h = -1;
    if (res.count(h)){
      res[h] += bdds[i];
    }else{
      res[h] = bdds[i];
    }
   
    not_reached *= !bdds[i];
  }
  if (res.count(-1)){
    res[-1] += not_reached;
  }else{
    res[-1] = not_reached;
  }
}


ADD SymbolicAbstraction::getExplicitHeuristic(){
  hash_map<int, BDD> hBDD;
  getHeuristicBDD(hBDD);  
  ADD h = manager->mgr().constant(0);
  for(hash_map<int, BDD>::iterator it = hBDD.begin(); it != hBDD.end(); ++it){
    int h_value = (*it).first;
    h += (*it).second.Add()*manager->mgr().constant(h_value);
  }
  cout << "ADD Heuristic size: " << h.nodeCount() << endl;
  return h;
}

void SymbolicAbstraction::storeExplicitHeuristic(string filename){
  hash_map<int, BDD> hBDD;
  getHeuristicBDD(hBDD);  
  for(hash_map<int, BDD>::iterator it = hBDD.begin(); it != hBDD.end(); ++it){
    int h_value = it->first;

    std::ostringstream ssBDD;
    ssBDD << filename << "_" << h_value<< endl;
    it->second.write(ssBDD.str());
  }
}


void SymbolicAbstraction::printBDDs(){
  for(int i = 0; i<  bdds.size(); i++){
    cout << "AbsState " << i << " s: " << endl;
    absStatePreBDD(i).print(1, 2);
    cout << "AbsState " << i << " sp: " << endl;
    absStateEffBDD(i).print(1, 2);
  }
}
