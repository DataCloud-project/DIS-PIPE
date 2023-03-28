#include "hm_heuristic.h"

#include "option_parser.h"
#include "plugin.h"

#include <cassert>
#include <limits>
#include <set>



HMHeuristic::HMHeuristic(const Options &opts)
    : Heuristic(opts),
      m(opts.get<int>("m")) {
}

HMHeuristic::~HMHeuristic() {
}

void HMHeuristic::initialize() {
    cout << "Using h^" << m << endl;
    cout << "The implementation of the h^m heuristic is preliminary" << endl
         << "It is SLOOOOOOOOOOOW" << endl
         << "Please do not use this for comparison" << endl;
    generate_all_hmtuples();
}

int HMHeuristic::compute_heuristic(const State &state) {
    if (test_goal(state)) {
        return 0;
    } else {
        hmtuple s_tup;
        state_to_hmtuple(state, s_tup);

        init_hm_table(s_tup);
        update_hm_table();
        //dump_table();

        int h = eval(g_goal);

        if (h == numeric_limits<int>::max())
            return DEAD_END;
        return h;
    }
}


void HMHeuristic::init_hm_table(hmtuple &t) {
    map<hmtuple, int>::iterator it;
    for (it = hm_table.begin(); it != hm_table.end(); it++) {
        pair<hmtuple, int> hm_ent = *it;
        hmtuple &tup = hm_ent.first;
        int h_val = check_hmtuple_in_hmtuple(tup, t);
        hm_table[tup] = h_val;
    }
}


void HMHeuristic::update_hm_table() {
    int round = 0;
    do {
        round++;
        was_updated = false;

        for (int op_id = 0; op_id < g_operators.size(); op_id++) {
            const Operator &op = g_operators[op_id];
            hmtuple pre;
            get_operator_pre(op, pre);

            int c1 = eval(pre);
            if (c1 != numeric_limits<int>::max()) {
                hmtuple eff;
                vector<hmtuple> partial_eff;
                get_operator_eff(op, eff);
                generate_all_partial_hmtuple(eff, partial_eff);
                for (int i = 0; i < partial_eff.size(); i++) {
                    update_hm_entry(partial_eff[i], c1 + get_adjusted_cost(op));

                    if (partial_eff[i].size() < m) {
                        extend_hmtuple(partial_eff[i], op);
                    }
                }
            }
        }
    } while (was_updated);
}

void HMHeuristic::extend_hmtuple(hmtuple &t, const Operator &op) {
    map<hmtuple, int>::const_iterator it;
    for (it = hm_table.begin(); it != hm_table.end(); it++) {
        pair<hmtuple, int> hm_ent = *it;
        hmtuple &entry = hm_ent.first;
        bool contradict = false;
        for (int i = 0; i < entry.size(); i++) {
            if (contradict_effect_of(op, entry[i].first, entry[i].second)) {
                contradict = true;
                break;
            }
        }
        if (!contradict && (entry.size() > t.size()) && (check_hmtuple_in_hmtuple(t, entry) == 0)) {
            hmtuple pre;
            get_operator_pre(op, pre);

            hmtuple others;
            for (int i = 0; i < entry.size(); i++) {
                pair<int, int> fact = entry[i];
                if (find(t.begin(), t.end(), fact) == t.end()) {
                    others.push_back(fact);
                    if (find(pre.begin(), pre.end(), fact) == pre.end()) {
                        pre.push_back(fact);
                    }
                }
            }

            sort(pre.begin(), pre.end());


            set<int> vars;
            bool is_valid = true;
            for (int i = 0; i < pre.size(); i++) {
                if (vars.count(pre[i].first) != 0) {
                    is_valid = false;
                    break;
                }
                vars.insert(pre[i].first);
            }

            if (is_valid) {
                int c2 = eval(pre);
                if (c2 != numeric_limits<int>::max()) {
                    update_hm_entry(entry, c2 + get_adjusted_cost(op));
                }
            }
        }
    }
}

int HMHeuristic::eval(hmtuple &t) {
    vector<hmtuple> partial;
    generate_all_partial_hmtuple(t, partial);
    int max = 0;
    for (int i = 0; i < partial.size(); i++) {
        assert(hm_table.count(partial[i]) == 1);

        int h = hm_table[partial[i]];
        if (h > max) {
            max = h;
        }
    }
    return max;
}

int HMHeuristic::update_hm_entry(hmtuple &t, int val) {
    assert(hm_table.count(t) == 1);
    if (hm_table[t] > val) {
        hm_table[t] = val;
        was_updated = true;
    }
    return val;
}

void HMHeuristic::generate_all_hmtuples(int var, int sz, hmtuple &base) {
    if (sz == 1) {
        for (int i = var; i < g_variable_domain.size(); i++) {
            for (int j = 0; j < g_variable_domain[i]; j++) {
                hmtuple tup(base);
                tup.push_back(make_pair(i, j));
                hm_table[tup] = 0;
            }
        }
    } else {
        for (int i = var; i < g_variable_domain.size(); i++) {
            for (int j = 0; j < g_variable_domain[i]; j++) {
                hmtuple tup(base);
                tup.push_back(make_pair(i, j));
                hm_table[tup] = 0;
                generate_all_hmtuples(i + 1, sz - 1, tup);
            }
        }
    }
}

void HMHeuristic::generate_all_partial_hmtuple(hmtuple &base_hmtuple, hmtuple &t,
                                             int index, int sz, vector<hmtuple> &res) {
    if (sz == 1) {
        for (int i = index; i < base_hmtuple.size(); i++) {
            hmtuple tup(t);
            tup.push_back(base_hmtuple[i]);
            res.push_back(tup);
        }
    } else {
        for (int i = index; i < base_hmtuple.size(); i++) {
            hmtuple tup(t);
            tup.push_back(base_hmtuple[i]);
            res.push_back(tup);
            generate_all_partial_hmtuple(base_hmtuple, tup, i + 1, sz - 1, res);
        }
    }
}


int HMHeuristic::check_hmtuple_in_hmtuple(const hmtuple &tup, const hmtuple &big_hmtuple) {
    for (int i = 0; i < tup.size(); i++) {
        bool found = false;
        for (int j = 0; j < big_hmtuple.size(); j++) {
            if (tup[i] == big_hmtuple[j]) {
                found = true;
                break;
            }
        }
        if (!found) {
            return numeric_limits<int>::max();
        }
    }
    return 0;
}

static ScalarEvaluator *_parse(OptionParser &parser) {
    parser.add_option<int>("m", 2);
    Heuristic::add_options_to_parser(parser);
    Options opts = parser.parse();
    if (parser.dry_run())
        return 0;
    else
        return new HMHeuristic(opts);
}


static Plugin<ScalarEvaluator> _plugin("hm", _parse);
