#include <iostream>
#include <vector>
#include <string>

#include "cmdline.h"

#include "main.h"
#include "graph.h"
#include "solver.h"


using namespace std;


int main(int argc, char *argv[]){
  // parser
  cmdline::parser op;
  op.add<string>("input", 'i', "input file name", false);
  op.add("help", 0, "print help");

  if (!op.parse(argc, argv)||op.exist("help")){
    std::cout<<op.error_full()<<op.usage();
    return 0;
  }

  // input
  Graph Gin;

  if(op.exist("input")){
    const char *fn = op.get<string>("input").c_str();
    Gin = Graph(fn);
  }
  else Gin = Graph("../instances/exact/exact001.gr");


  Gin.show();

  vector <edge> sol;
  cout << "ans: " << naive_branching(Gin, Gin, 100, sol) << endl;


  return 0;

}
