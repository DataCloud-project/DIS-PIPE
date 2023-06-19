#ifndef STATE_H
#define STATE_H

#include <iostream>
#include <map>
#include <vector>
using namespace std;

class Variable;

class State {
    map<const Variable *, int> values;
public:
    State() {} // TODO: Entfernen (erfordert kleines Redesign)
    State(istream &in, const vector<Variable *> &variables);

    int operator[](const Variable *var) const;
    void dump() const;
    void remove_unreachable_facts();
};

#endif
