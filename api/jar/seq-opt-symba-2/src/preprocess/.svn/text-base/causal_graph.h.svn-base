#ifndef CAUSAL_GRAPH_H
#define CAUSAL_GRAPH_H

#include <iosfwd>
#include <vector>
#include <map>
#include "rng.h"
using namespace std;

class Operator;
class Axiom;
class Variable;

class CausalGraph {
    const vector<Variable *> &variables;
    const vector<Operator> &operators;
    const vector<Axiom> &axioms;
    const vector<pair<Variable *, int> > &goals;

    typedef map<Variable *, int> WeightedSuccessors;
    typedef map<Variable *, WeightedSuccessors> WeightedGraph;
    WeightedGraph weighted_graph;
    typedef map<Variable *, int> Predecessors;
    typedef map<Variable *, Predecessors> PredecessorGraph;
    // predecessor_graph is weighted_graph with edges turned around
    PredecessorGraph predecessor_graph;

    typedef vector<vector<Variable *> > Partition;
    typedef vector<Variable *> Ordering;
    Ordering ordering;
    bool acyclic;
    

    RandomNumberGenerator rng;

    //atorralba: new attributes for ease the variable order optimizatuion
    Partition sccs; //atorralba: Store sccs as an attribute in the causal graph    
    map <Variable *, int> scc_var; //atorralba: Access the index of the scc of the variable 
    //atorralba: If there is an edge between each pair of sccs, their order cannot be exchanged 
    vector <vector <bool> > conflict_scc; 
    //atorralba: influence of each variable in another.
    vector<vector<bool> > influence_graph; 
    

    void influence_graph_from_ops(const vector<Variable *> &variables,
				  const vector<Operator> &operators,
				  const vector<pair<Variable *, int> > &goals);

    void weigh_graph_from_ops(const vector<Variable *> &variables,
                              const vector<Operator> &operators,
                              const vector<pair<Variable *, int> > &goals);
    void weigh_graph_from_axioms(const vector<Variable *> &variables,
                                 const vector<Axiom> &axioms,
                                 const vector<pair<Variable *, int> > &goals);
    void get_strongly_connected_components(const vector<Variable *> &variables, Partition &sccs);
    void calculate_topological_pseudo_sort(const Partition &sccs);
    void calculate_important_vars();
    void dfs(Variable *from);
public:
    CausalGraph(const vector<Variable *> &variables,
                const vector<Operator> &operators,
                const vector<Axiom> &axioms,
                const vector<pair<Variable *, int> > &the_goals);
    ~CausalGraph() {}
    const vector<Variable *> &get_variable_ordering() const;
    bool is_acyclic() const;
    void dump() const;
    void generate_cpp_input(ofstream &outfile,
                            const vector<Variable *> &ordered_vars) const;

    void update();


    //void optimize_ordering(); //Applies the variable optimization algorithm
    void optimize_ordering_gamer(); //Applies the variable optimization algorithm

    //Changes the order of variables in ordering by swaping two sccs.
    void swap_scc(vector <Variable * > & result);
    //Applies the variable ordering optimization, swaping variables in the same scc.
    //long optimize_variable_ordering(vector <Variable *> & order, int iterations);
    long optimize_variable_ordering_gamer(vector <Variable *> & order, int iterations);

    //Returns the value of the optimization function for a particular ordering
    long compute_function(vector <Variable *> & order);
    //Returns the influence between two variables.
    long influence(Variable * v1, Variable * v2);

};

extern bool g_do_not_prune_variables;

#endif
