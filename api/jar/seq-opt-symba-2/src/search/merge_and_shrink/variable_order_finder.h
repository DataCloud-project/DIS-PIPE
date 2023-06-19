#ifndef MERGE_AND_SHRINK_VARIABLE_ORDER_FINDER_H
#define MERGE_AND_SHRINK_VARIABLE_ORDER_FINDER_H

#include <vector>

//Alvaro: MergeStrategy moved here from merge_and_shrink_heuristic
enum MergeStrategy {
    MERGE_LINEAR_CG_GOAL_LEVEL,
    MERGE_LINEAR_CG_GOAL_RANDOM,
    MERGE_LINEAR_GOAL_CG_LEVEL,
    MERGE_LINEAR_RANDOM,
    MERGE_DFP,
    MERGE_LINEAR_LEVEL,
    MERGE_LINEAR_REVERSE_LEVEL
};


class VariableOrderFinder {
    const MergeStrategy merge_strategy;
    std::vector<int> selected_vars;
    std::vector<int> remaining_vars;
    std::vector<bool> is_goal_variable;
    std::vector<bool> is_causal_predecessor;

    void select_next(int position, int var_no);
public:
    VariableOrderFinder(MergeStrategy merge_strategy, bool is_first = true);
    VariableOrderFinder(MergeStrategy merge_strategy_, bool is_first,
			const std::vector <int> & remaining_vars_);
    ~VariableOrderFinder();
    bool done() const;
    int next();
};

#endif
