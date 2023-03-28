#ifndef MERGE_AND_SHRINK_SHRINK_BUCKET_BASED_H
#define MERGE_AND_SHRINK_SHRINK_BUCKET_BASED_H

#include "shrink_strategy.h"

#include <vector>


class Options;

class ShrinkAll : public ShrinkStrategy {

public:
    ShrinkAll(const Options &opts);
    virtual ~ShrinkAll();

    void shrink_before_merge(SymbolicAbstraction &abs1, SymbolicAbstraction &abs2);

    virtual bool reduce_labels_before_shrinking() const;

    virtual void shrink(SymbolicAbstraction &abs,
			int threshold,
                        bool force = false);

    virtual std::string name() const;

};

#endif
