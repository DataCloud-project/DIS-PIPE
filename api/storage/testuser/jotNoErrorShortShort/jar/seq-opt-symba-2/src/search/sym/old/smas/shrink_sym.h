#ifndef MERGE_AND_SHRINK_SHRINK_SYM_H
#define MERGE_AND_SHRINK_SHRINK_SYM_H

#include "shrink_strategy.h"

class Options;
class Signature;

class ShrinkSymbolic : public ShrinkStrategy {

  const bool group_by_h;

    void compute_abstraction(
        SymbolicAbstraction &abs,
        int target_size,
        EquivalenceRelation &equivalence_relation);

    int initialize_groups(const SymbolicAbstraction &abs,
                          std::vector<int> &state_to_group);
    void compute_signatures(
        const SymbolicAbstraction &abs,
        std::vector<Signature> &signatures,
        std::vector<int> &state_to_group);
public:
    ShrinkSymbolic(const Options &opts);
    virtual ~ShrinkSymbolic();

    virtual std::string name() const;
    virtual void dump_strategy_specific_options() const;

    virtual bool reduce_labels_before_shrinking() const;

    virtual void shrink(SymbolicAbstraction &abs, int target, bool force = false);
    virtual void shrink_before_merge(SymbolicAbstraction &abs1,
				     SymbolicAbstraction &abs2);

    static ShrinkStrategy *create_default();
};

#endif
