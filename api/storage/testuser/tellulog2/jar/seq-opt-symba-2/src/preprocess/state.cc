#include "state.h"
#include "helper_functions.h"

class Variable;

State::State(istream &in, const vector<Variable *> &variables) {
    check_magic(in, "begin_state");
    for (int i = 0; i < variables.size(); i++) {
        int value;
        in >> value; //for axioms, this is default value
        values[variables[i]] = value;
    }
    check_magic(in, "end_state");
}

int State::operator[](const Variable *var) const {
    return values.find(var)->second;
}

void State::dump() const {
  for (map<const Variable *, int>::const_iterator it = values.begin();
       it != values.end(); ++it)
    cout << "  " << it->first->get_name() << ": " << it->second << endl;
}

void State::remove_unreachable_facts(){
  map<const Variable *, int> newvalues;
  for (map<const Variable *, int>::const_iterator it = values.begin();
       it != values.end(); ++it){
    if(it->first->is_necessary())
      newvalues[it->first] = it->first->get_new_id(it->second);
  }
  newvalues.swap(values);
}
