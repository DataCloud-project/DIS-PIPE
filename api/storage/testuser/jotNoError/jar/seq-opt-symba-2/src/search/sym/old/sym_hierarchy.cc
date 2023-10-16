#include "sym_hierarchy.h"
#include "sym_ph.h"
#include "sym_engine.h"

using namespace std;

SymHierarchy::SymHierarchy() : root(nullptr){}

void SymHierarchy::init (SymEngine * eng,
			 SymPH * policy){
  engine = eng;
  vars = engine->getVars();
  policyHierarchy = policy;
}


SymHNode * SymHierarchy::abstract(SymHNode * node,
				  std::unique_ptr<SymAbstraction> newAbstraction){
  SymHNode * newNode = new SymHNode(node, move (newAbstraction));
  nodes.push_back(unique_ptr<SymHNode> (newNode));
  node->addChildren(newNode);
  newNode->addParent(node);
  return newNode;
}

SymHNode * SymHierarchy::concrete(SymHNode * node, std::unique_ptr<SymAbstraction> newAbstraction){
  SymHNode * newNode = new SymHNode(node, move (newAbstraction));
  nodes.push_back(unique_ptr<SymHNode> (newNode));
  newNode->addChildren(node);
  node->addParent(newNode);
  return node;
}

SymBDExp * SymHierarchy::load_exploration(Dir searchDir, const string & filename){
  SymBDExp * refExp = new SymBDExp(engine, searchDir, filename, root);
  root->add_exploration(refExp);
  return refExp;
}

SymBDExp * SymHierarchy::init_exploration(Dir searchDir){
  SymBDExp * refExp = new SymBDExp(engine, searchDir);
  if(refExp->initFrontier(root, numeric_limits<int>::max(), numeric_limits<int>::max()) &&
     refExp->initAll(numeric_limits<int>::max(), numeric_limits<int>::max())){
    root->add_exploration(refExp);
    return refExp;
  }else{
    delete refExp;
    cout << "Init exploration failed" << endl;
    return nullptr;
  }
}

SymBDExp * SymHierarchy::relax(SymBDExp * bdExp, Dir dir,
			       int maxStepTime,  int maxStepNodes,
			       double ratioUseful){
  return policyHierarchy->relax(bdExp, dir,
				maxStepTime, maxStepNodes, ratioUseful);
}
