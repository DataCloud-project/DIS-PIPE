0#include "sym_psel.h"
#include "sym_hierarchy.h"


SymPSel::SymPSel(const Options & opts):searchDir(Dir(opts.get_enum("search_dir"))),
				       loadExploration (opts.get<bool>("load")),
				       saveExploration (opts.get<bool>("save")) {
}

void SymPSel::init(SymHierarchy * hierarchy){
}

void SymPSel::set_default_options(Options & opts){
  opts.set<bool>("load", false);
  opts.set<bool>("save", false);
}

void SymPSel::add_options_to_parser(OptionParser &parser){
  parser.add_option<bool>("load", false,
			 "load exploration from file");
  parser.add_option<bool>("save", false,
			 "save exploration to file");
}
