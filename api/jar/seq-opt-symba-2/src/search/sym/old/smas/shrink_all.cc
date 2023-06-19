#include "shrink_all.h"

#include "symbolic_abstraction.h"

#include "../option_parser.h"
#include "../plugin.h"

#include <cassert>
#include <iostream>
#include <vector>
using namespace std;


ShrinkAll::ShrinkAll(const Options &opts)
    : ShrinkStrategy(opts) {
}

ShrinkAll::~ShrinkAll() {
}

bool ShrinkAll::reduce_labels_before_shrinking() const {
    return false;
}

void ShrinkAll::shrink_before_merge(SymbolicAbstraction &abs1, SymbolicAbstraction &/*abs2*/){
  shrink(abs1, 1);

}

void ShrinkAll::shrink(SymbolicAbstraction &abs,
		       int threshold, bool force) {
    if (must_shrink(abs, threshold, force)) {
	EquivalenceClass group;
	for(int i = 0; i < abs.size(); i++) {
	  group.push_front(i);
	}
        EquivalenceRelation equiv_relation(1, group);

        apply(abs, equiv_relation, threshold);
    }
}

string ShrinkAll::name() const{
  return "shrink_all";
}

static ShrinkStrategy *_parse(OptionParser &parser) {
    ShrinkStrategy::add_options_to_parser(parser);
    Options opts = parser.parse();
    ShrinkStrategy::handle_option_defaults(opts);

    if (!parser.dry_run())
        return new ShrinkAll(opts);
    else
        return 0;
}

static Plugin<ShrinkStrategy> _plugin("shrink_all", _parse);

