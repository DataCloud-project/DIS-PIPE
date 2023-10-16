#ifndef SYMBOLIC_ABSTRACTION_H
#define SYMBOLIC_ABSTRACTION_H

#include "./symbolic_exploration/sym_manager.h"
#include "shrink_strategy.h"

#include "../operator_cost.h"

#include <ext/slist>
#include <ext/hash_map>

#include <vector>
using namespace std;
using namespace __gnu_cxx;

class State;
class Operator;

class AbstractTransition {
 public:
    AbstractStateRef src;
    AbstractStateRef target;

    AbstractTransition(AbstractStateRef src_, AbstractStateRef target_)
        : src(src_), target(target_) {
    }

    bool operator==(const AbstractTransition &other) const {
        return src == other.src && target == other.target;
    }

    bool operator!=(const AbstractTransition &other) const {
        return !(*this == other);
    }

    bool operator<(const AbstractTransition &other) const {
        return src < other.src || (src == other.src && target < other.target);
    }
};

class SymExploration;

class SymbolicAbstraction {
    friend class ShrinkStrategy; // for apply() -- TODO: refactor!

    static const int PRUNED_STATE = -1;
    static const int DISTANCE_UNKNOWN = -2;

    SymManager * manager;
    const bool is_unit_cost;
    const OperatorCost cost_type;

    SymExploration * exploration;

    vector<BDD> absVarsS, absVarsSp;
    BDD existAbsStateVars;
    BDD biimpAbsStateVars;
    vector<BDD> absBDDs; //BDDs describing each abstract state

    vector<const Operator *> relevant_operators;
    vector<const Operator *> irrelevant_operators;
    int num_states;
    vector<vector<AbstractTransition> > transitions_by_op;

    vector<int> init_distances;
    vector<int> goal_distances;
    vector<bool> goal_states;
    AbstractStateRef init_state;

    int max_f;
    int max_g;
    int max_h;

    bool are_labels_reduced;

    mutable int peak_memory;

    void clear_distances();
    void compute_init_distances_unit_cost();
    void compute_goal_distances_unit_cost();
    void compute_init_distances_general_cost();
    void compute_goal_distances_general_cost();

    void apply_abstraction(vector<slist<AbstractStateRef> > &collapsed_groups);

    int total_transitions() const;
    int unique_unlabeled_transitions() const;
protected:
    vector<int> varset;

    //    virtual AbstractStateRef get_abstract_state(const State &state) const = 0;
    /*    virtual void apply_abstraction_to_lookup_table(const vector<
	  AbstractStateRef> &abstraction_mapping) = 0;*/
        virtual int memory_estimate() const;
public:
    SymbolicAbstraction(SymManager * mgr, bool is_unit_cost_,
			OperatorCost cost_type_, int variable);
    SymbolicAbstraction(SymManager * mgr, bool is_unit_cost_,
			OperatorCost cost_type_,
			SymbolicAbstraction *abs1, SymbolicAbstraction *abs2);
    virtual ~SymbolicAbstraction();

    // Two methods to identify the abstraction in output.
    // tag is a convience method that upper-cases the first letter of
    // description and appends ": ";
    //    virtual std::string description() const = 0;
    std::string tag() const;

    static void build_atomic_abstractions(SymManager * mgr,
        bool is_unit_cost_, OperatorCost cost_type_,
        std::vector<SymbolicAbstraction *> &result);
    bool is_solvable() const;

    int size() const;
    void statistics(bool include_expensive_statistics) const;

    int get_peak_memory_estimate() const;
    // NOTE: This will only return something useful if the memory estimates
    //       have been computed along the way by calls to statistics().
    // TODO: Find a better way of doing this that doesn't require
    //       a mutable attribute?

    bool is_in_varset(int var) const;

    void compute_distances();
    void normalize(bool reduce_labels);
    void release_memory();

    void dump() const;

    // The following methods exist for the benefit of shrink strategies.
    int get_max_f() const;
    int get_max_g() const;
    int get_max_h() const;

    bool is_goal_state(int state) const {
        return goal_states[state];
    }

    int get_init_distance(int state) const {
        return init_distances[state];
    }

    int get_goal_distance(int state) const {
        return goal_distances[state];
    }

    inline int get_num_ops() const {
        return transitions_by_op.size();
    }

    const std::vector<AbstractTransition> &get_transitions_for_op(int op_no) const {
        return transitions_by_op[op_no];
    }

    int get_cost_for_op(int op_no) const;

    inline int get_num_relevant_ops() const {
        return relevant_operators.size();
    }

    inline const Operator * get_relevant_operator(int op_no) const {
      return relevant_operators[op_no];
    }

    inline int get_num_irrelevant_ops() const {
        return irrelevant_operators.size();
    }

    inline const Operator * get_irrelevant_operator(int op_no) const {
      return irrelevant_operators[op_no];
    }

    inline const vector <BDD> &getAbsVarsS() const {
      return absVarsS;
    }

    inline const vector <BDD> &getAbsVarsSp() const {
      return absVarsSp;
    }

    inline BDD getBiimpAbsStateVars() const {
      return biimpAbsStateVars;
    }


    inline BDD absStatePreBDD(AbstractStateRef state) const {
      return absBDDs[state];
    }

    inline BDD absStateEffBDD(AbstractStateRef state) const{
      return absBDDs[state].SwapVariables(absVarsS, absVarsSp);
    }

    inline void set_exploration(SymExploration * exp){
      exploration = exp;
    }

    inline SymExploration * get_exploration(){
      return exploration;
    }
 
    
    //Returns the heuristic in the form of an ADD
    ADD getHeuristicADD(); 
    void getHeuristicBDD(hash_map<int, BDD> & res);
    ADD getExplicitHeuristic(); 

    void printBDDs();

    void storeExplicitHeuristic(string filename);
};


#endif
