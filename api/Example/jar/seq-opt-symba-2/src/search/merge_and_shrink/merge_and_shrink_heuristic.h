#ifndef MERGE_AND_SHRINK_MERGE_AND_SHRINK_HEURISTIC_H
#define MERGE_AND_SHRINK_MERGE_AND_SHRINK_HEURISTIC_H

#include "shrink_strategy.h"

#include "../heuristic.h"

#include "variable_order_finder.h" // needed for MergeStrategy type;
// TODO: move that type somewhere else?

#include <utility>
#include <vector>

class Abstraction;

class MergeAndShrinkHeuristic : public Heuristic {
    const int abstraction_count;
    const MergeStrategy merge_strategy;
    ShrinkStrategy *const shrink_strategy;
    const bool use_label_reduction;
    const bool use_expensive_statistics;

    std::vector<Abstraction *> abstractions;
    Abstraction *build_abstraction(bool is_first = true);

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
