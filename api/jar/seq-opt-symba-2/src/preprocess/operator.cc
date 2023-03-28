#include "helper_functions.h"
#include "operator.h"
#include "variable.h"
#include "h2_mutexes.h"
#include <cassert>
#include <iostream>
#include <fstream>
using namespace std;

Operator::Operator(istream &in, const vector<Variable *> &variables) : spurious(false) {
    check_magic(in, "begin_operator");
    in >> ws;
    getline(in, name);
    int count; // number of prevail conditions
    in >> count;
    for (int i = 0; i < count; i++) {
        int varNo, val;
        in >> varNo >> val;
        prevail.push_back(Prevail(variables[varNo], val));
    }
    in >> count; // number of pre_post conditions
    for (int i = 0; i < count; i++) {
        int eff_conds;
        vector<EffCond> ecs;
        in >> eff_conds;
        for (int j = 0; j < eff_conds; j++) {
            int var, value;
            in >> var >> value;
            ecs.push_back(EffCond(variables[var], value));
        }
        int varNo, val, newVal;
        in >> varNo >> val >> newVal;
        if (eff_conds)
            pre_post.push_back(PrePost(variables[varNo], ecs, val, newVal));
        else
            pre_post.push_back(PrePost(variables[varNo], val, newVal));
    }
    in >> cost;
    check_magic(in, "end_operator");
    // TODO: Evtl. effektiver: conditions schon sortiert einlesen?
}

void Operator::dump() const {
    cout << name << ":" << endl;
    cout << "prevail:";
    for (int i = 0; i < prevail.size(); i++)
        cout << "  " << prevail[i].var->get_name() << " := " << prevail[i].prev;
    cout << endl;
    cout << "pre-post:";
    for (int i = 0; i < pre_post.size(); i++) {
        if (pre_post[i].is_conditional_effect) {
            cout << "  if (";
            for (int j = 0; j < pre_post[i].effect_conds.size(); j++)
                cout << pre_post[i].effect_conds[j].var->get_name() << " := " <<
                pre_post[i].effect_conds[j].cond;
            cout << ") then";
        }
        cout << " " << pre_post[i].var->get_name() << " : " <<
        pre_post[i].pre << " -> " << pre_post[i].post;
    }
    cout << endl;
}

int Operator::get_encoding_size() const {
    int size = 1 + prevail.size();
    for (int i = 0; i < pre_post.size(); i++) {
        size += 1 + pre_post[i].effect_conds.size();
        if (pre_post[i].pre != -1)
            size += 1;
    }
    return size;
}

void Operator::strip_unimportant_effects() {
    int new_index = 0;
    for (int i = 0; i < pre_post.size(); i++) {
        if (pre_post[i].var->get_level() != -1)
            pre_post[new_index++] = pre_post[i];
    }
    pre_post.erase(pre_post.begin() + new_index, pre_post.end());
}

bool Operator::is_redundant() const {
    return spurious || pre_post.empty();
}

void strip_operators(vector<Operator> &operators) {
    int old_count = operators.size();
    int new_index = 0;
    for (int i = 0; i < operators.size(); i++) {
        operators[i].strip_unimportant_effects();
        if (!operators[i].is_redundant())
            operators[new_index++] = operators[i];
    }
    operators.erase(operators.begin() + new_index, operators.end());
    cout << operators.size() << " of " << old_count << " operators necessary." << endl;
}

void Operator::generate_cpp_input(ofstream &outfile) const {
    //TODO: beim Einlesen in search feststellen, ob leerer Operator
    outfile << "begin_operator" << endl;
    outfile << name << endl;

  outfile << prevail.size() << endl;
  for (int i = 0; i < prevail.size(); i++) {
    assert(prevail[i].var->get_level() != -1);
    if (prevail[i].var->get_level() != -1)
      outfile << prevail[i].var->get_level() << " " << prevail[i].prev << endl;
  }

  outfile << pre_post.size() << endl;
  for (int i = 0; i < pre_post.size(); i++) {
    assert(pre_post[i].var->get_level() != -1);
    outfile << pre_post[i].effect_conds.size();
    for (int j = 0; j < pre_post[i].effect_conds.size(); j++)
      outfile << " " << pre_post[i].effect_conds[j].var->get_level()
	      << " " << pre_post[i].effect_conds[j].cond;
    outfile << " " << pre_post[i].var->get_level()
	    << " " << pre_post[i].pre
	    << " " << pre_post[i].post << endl;
  }
  outfile << cost << endl;
  outfile << "end_operator" << endl;
}

// Removes ambiguity in the preconditions,
// detects whether the operator is spurious
void Operator::remove_ambiguity(const H2Mutexes & h2) {
    if (is_redundant())
        return;
    // cout << "Check ambiguity: " << name << endl;
  
    vector<int> preconditions(h2.num_variables(), -1);
    vector<bool> original(h2.num_variables(), false);

    vector<bool> effect_var(h2.num_variables(), false);
    vector<pair<int, int> > effects;

    vector<pair<int, int> > known_values;

    for (int i = 0; i < prevail.size(); i++){
        int var = prevail[i].var->get_level();
        if(var != -1){
            preconditions[var] = prevail[i].prev;
            known_values.push_back(make_pair(var, prevail[i].prev));
            original[var] = true;
        }
    }
    for (int i = 0; i < pre_post.size(); i++) {
        int var = pre_post[i].var->get_level();
        if (var != -1) {
            preconditions[var] = pre_post[i].pre;
            known_values.push_back(make_pair(var, pre_post[i].pre));
            original[var] = (preconditions[var] != -1);
            effect_var[var] = true;
            effects.push_back(make_pair(var, pre_post[i].post));
        }
    }
    for (int i = 0; i < augmented_preconditions.size(); i++) {
        preconditions[augmented_preconditions[i].first] = augmented_preconditions[i].second;
        known_values.push_back(make_pair(augmented_preconditions[i].first, augmented_preconditions[i].second));
        original[augmented_preconditions[i].first] = true;
    }

    // check that no precondition is unreachable or mutex with some other precondition
    for (int i = 0; i < preconditions.size(); i++) {
        if (preconditions[i] != -1) {
            if (h2.is_unreachable(i,preconditions[i])) {
                spurious = true;
                return;
            }
            for (int j = i+1; j < preconditions.size(); j++) {
                if (h2.are_mutex(i, preconditions[i], j, preconditions[j])) {
                    spurious = true;
                    return;
                }
            }
        }
    }

    std::list<std::pair<unsigned, std::list<unsigned> > > candidates;
    for (int i = 0; i < h2.num_variables(); i++) {
        // consider unknown preconditions only
        if (preconditions[i] != -1)
            continue;

        pair<unsigned, list<unsigned> > candidate_var = make_pair(i, list<unsigned>());
        // add every reachable fluent
        for (int j = 0; j < h2.num_values(i); j++)
            candidate_var.second.push_back(j);

        candidates.push_back(candidate_var);
    }

    // actual disambiguation process
    while (!known_values.empty()) {
        vector<pair<int, int> > aux_values;
        // for each unknown variable
        for (list<pair<unsigned, list<unsigned> > >::iterator it = candidates.begin(); it != candidates.end();) {
            unsigned var = it->first;
            list<unsigned> candidate_var = it->second;
            // cout << var << " -> " << candidate_var.size() << endl;

            // we eliminate candidates mutex with other things
            for (list<unsigned>::iterator it2 = candidate_var.begin(); it2 != candidate_var.end();) {
                bool mutex = h2.is_unreachable(var,*it2);
                for (int k = 0; !mutex && k < known_values.size(); k++)
                    mutex = h2.are_mutex(known_values[k].first, known_values[k].second, var,*it2);

                if (!effect_var[var]) {
                   for (int k = 0; !mutex && k < effects.size(); k++)
                       mutex = h2.are_mutex(effects[k].first, effects[k].second, var,*it2);
                }



                if (mutex) {
                    it2 = candidate_var.erase(it2);
                } else {
                    ++it2;
                }   
            }

            // we check the remaining candidates
            if (candidate_var.empty()) { // if no fluent is possible for a given variable, the operator is spurious
                spurious = true;
                return;
            } else if (candidate_var.size() == 1) { // add the single possible fluent to preconditions and aux_values and remove the variables from candidate
                pair<int, int> new_fluent = make_pair(var,candidate_var.front());
                aux_values.push_back(new_fluent);
                preconditions[new_fluent.first] = new_fluent.second;
                it = candidates.erase(it);
            } else {
                ++it;
            }
        }

        known_values.swap(aux_values);
    }
    

    // new preconditions are added
    for (int i = 0; i < preconditions.size(); i++)
        if (preconditions[i] != -1 && !original[i])
            augmented_preconditions.push_back(make_pair(i,preconditions[i]));

    // potential preconditions are set
    // important for backwards h^2
    // note: they may overlap with augmented preconditions
    potential_preconditions.clear();
    for (int i = 0; i < pre_post.size(); i++) {
        // for each undefined precondition
        if (pre_post[i].pre != -1)
            continue;

        int var = pre_post[i].var->get_level();
        if (preconditions[var] != -1){
            potential_preconditions.push_back(make_pair(var, preconditions[var]));
            continue;
        }

        // for each fluent
        for (int j = 0; j < h2.num_values(var); j++) {
            bool conflict = false;
            for (int k = 0; !conflict && k < preconditions.size(); k++)
                if (preconditions[k] != -1)
                    conflict = h2.are_mutex(var, j, i, preconditions[i]);

            if (!conflict)
                potential_preconditions.push_back(make_pair(var,j));
        }
    }
}


void Operator::remove_unreachable_facts(){
  vector<Prevail> newprev;
  for(int i = 0; i < prevail.size(); ++i){
    if(prevail[i].var->is_necessary()) {
      prevail[i].remove_unreachable_facts();
      newprev.push_back(prevail[i]);
    }
  }
  newprev.swap(prevail);
  for(int i = 0; i < pre_post.size(); ++i){
    pre_post[i].remove_unreachable_facts();
  }
}
