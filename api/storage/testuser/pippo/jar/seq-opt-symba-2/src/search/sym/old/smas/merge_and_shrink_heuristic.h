#ifndef MERGE_AND_SHRINK_MERGE_AND_SHRINK_HEURISTIC_H
#define MERGE_AND_SHRINK_MERGE_AND_SHRINK_HEURISTIC_H

#include "shrink_strategy.h"
#include "symbolic_exploration/sym_manager.h"
#include "../heuristic.h"

#include <utility>
#include <vector>

class SymbolicAbstraction;

enum MergeStrategy {
    MERGE_LINEAR_CG_GOAL_LEVEL,
    MERGE_LINEAR_CG_GOAL_RANDOM,
    MERGE_LINEAR_GOAL_CG_LEVEL,
    MERGE_LINEAR_RANDOM,
    MERGE_DFP,
    MERGE_LINEAR_LEVEL,
    MERGE_LINEAR_REVERSE_LEVEL,
    MAX_MERGE_STRATEGY
};



class MergeAndShrinkHeuristic : public Heuristic {
    const int abstraction_count;
    const int max_tr_size;   //Maximum nodes for search frontier
    const int max_tr_merge_time;   //Maximum nodes for search frontier
    const int max_frontier_size;   //Maximum nodes for search frontier
    const int max_image_size;      //Maximum nodes for auxiliary image
    const int max_mas_time; //Maximum time for doing symbolic searches
    const int max_sym_time; //Maximum time for doing symbolic searches
    const int max_exploration_time;//Maximum time for each exploration
    const int max_image_time;      //Maximum time for each exploration
    const double reduce_frontier_factor;
    const bool restart_search;     //Restart search or not
    const bool restart_closed;     //Restart closed list or not
    const bool abstract_transitions; //Abstract transitions or not
    const MergeStrategy merge_strategy;
    ShrinkStrategy *const shrink_strategy;
    SymManager symbolicManager;
    const bool use_h2_mutex;
    const MutexType mutex_type;
    const bool use_label_reduction;
    const bool use_expensive_statistics;
    const bool write_to_disk; //If bdds should be written to disk


    // atorralb We only keep an ADD instead of an abstraction
    //std::vector<Abstraction *> abstractions;
    std::vector<ADD> heuristics;
    std::vector<int> max_heuristic_value;
    //Returns if the problem is solvablem
    bool build_abstraction(bool is_first, int * peak_mem);

    void dump_options() const;
    void warn_on_unusual_options() const;

protected:
    virtual void initialize();
    virtual int compute_heuristic(const State &state);
public:
    MergeAndShrinkHeuristic(const Options &opts);
    ~MergeAndShrinkHeuristic();
};

#endif
