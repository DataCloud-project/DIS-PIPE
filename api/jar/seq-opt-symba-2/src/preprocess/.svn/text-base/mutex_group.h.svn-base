#ifndef MUTEX_GROUP_H
#define MUTEX_GROUP_H

#include <iostream>
#include <vector>
#include "operator.h"
#include "state.h"
using namespace std;

class Variable;

enum Dir {FW, BW};

class MutexGroup {
  //Direction of the mutex. 
  // Fw mutexes are not reachable from the initial state (should be pruned in bw search)
  // Bw mutexes cannot reach the goal (should be pruned in fw search)
  // Both mutex groups contain fw and bw mutexes so they should be pruned in both directions
  Dir dir; 
  bool is_exactly_invariant; 
  vector<pair<const Variable *, int> > facts;
 public:
  MutexGroup(istream &in, const vector<Variable *> &variables);

  MutexGroup(const vector<pair<int, int> > & f,
	     const vector<Variable *> &variables,
	     bool regression);

  MutexGroup(const Variable * var);

  void strip_unimportant_facts();
  bool is_redundant() const;

  int get_encoding_size() const;
  void generate_cpp_input(ofstream &outfile) const;
  void dump() const;

  void get_invariant_group(vector<pair<int, int> > & invariant_group) const;

  void remove_unreachable_facts();
  void set_exactly_invariant(const vector<Operator> & operators,
				    const State & initial_state);

  void generate_gamer_input(ofstream &outfile) const;

  bool hasPair(int var, int val) const;

  inline const vector<pair<const Variable *, int> > & getFacts() const{
    return facts;
  }
};

extern void strip_mutexes(vector<MutexGroup> &mutexes);

//It may merge mutexes to get invariants
extern void generate_invariants(vector<MutexGroup> & mutexes, 
				const vector<Operator> & operators, 
				const State & initial_state);

#endif
