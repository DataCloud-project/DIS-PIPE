
#include "sym_heuristic.h"

std::ostream & operator<<(std::ostream &os, const SymHeuristic & h){
  h.print(os);
  return os;
}
