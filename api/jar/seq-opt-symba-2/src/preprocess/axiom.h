#ifndef AXIOM_H
#define AXIOM_H

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "variable.h"
using namespace std;

class Variable;

class Axiom {
public:
  class Condition {
  public: 
        Variable *var;
        int cond;
        Condition(Variable *v, int c) : var(v), cond(c) {}
	bool unreachable() const {
	  return !var->is_reachable(cond);
	}
	void remove_unreachable_facts(){
	  cond = var->get_new_id(cond);
	}
    };
private:
    Variable *effect_var;
    int old_val;
    int effect_val;
    vector<Condition> conditions;    // var, val
public:
    Axiom(istream &in, const vector<Variable *> &variables);

    bool is_redundant() const;
    void dump() const;
    int get_encoding_size() const;
    void generate_cpp_input(ofstream &outfile) const;
    const vector<Condition> &get_conditions() const {return conditions; }
    Variable *get_effect_var() const {return effect_var; }
    int get_old_val() const {return old_val; }
    int get_effect_val() const {return effect_val; }
    void remove_unreachable_facts();
};

extern void strip_axioms(vector<Axiom> &axioms);

#endif
