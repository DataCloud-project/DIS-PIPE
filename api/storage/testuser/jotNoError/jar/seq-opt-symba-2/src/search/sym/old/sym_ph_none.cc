#include "sym_ph_none.h"

SymPHNone::SymPHNone(const Options &opts) : 
  SymPH(opts){
}


unique_ptr<SymAbstraction> SymPHNone::abstract(SymAbstraction * /*abstraction*/){
  return nullptr;
}

/*unique_ptr<SymAbstraction> SymPHNone::concrete(SymAbstraction * abstraction){
  return nullptr;
}*/


/*std::unique_ptr<SymAbstraction> 
SymPHNone::noneAbstraction(SymVariables * vars){
  set<int> selectedVars;
  for(int i = 0; i < g_variable_name.size(); i++){
    selectedVars.insert(i);
  }
  return unique_ptr<SymAbstraction> (new SymPDB(vars, selectedVars));
}*/

std::unique_ptr<SymAbstraction> 
SymPHNone::noneAbstraction(){
  return nullptr;
}


int SymPHNone::numPossibleAbstractions(SymAbstraction * /*abstraction*/){
  return 0;
}

void SymPHNone::getListAbstraction(SymHNode * /*hNode*/, vector<SymHNode *> & /*res*/){
}

SymPH * SymPHNone::create_default(){
  Options opts;
  opts.set<int>("max_relax_time", 10e3);
  opts.set<int>("max_relax_nodes", 5e6);

  opts.set<bool>("perimeter_pdbs", false);
  opts.set<int>("relax_strategy", MANY_BINARY);
  opts.set<int>("relax_abs_strategy", MANY_LINEAR);
  opts.set<int>("abs_trs_strategy", IND_TR_SHRINK);
  return new SymPHNone(opts);
}

static SymPH *_parse(OptionParser &parser) {
  SymPH::add_options_to_parser(parser);
  Options opts = parser.parse();
  
  SymPH *policy = 0;
  if (!parser.dry_run()) {
    policy = new SymPHNone(opts);
  }
  
  return policy;
}

static Plugin<SymPH> _plugin("no", _parse);
