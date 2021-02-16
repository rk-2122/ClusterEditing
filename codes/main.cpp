#include <iostream>
#include <vector>

#include "cmdline.h"

#include "main.h"
#include "graph.h"
#include "solver.h"


using namespace std;


int main(int argc, char *argv[]){
  // parser
  cmdline::parser op;
  op.add<char *>("input", 'i', "input file name", false);
  op.add("help", 0, "print help");

  if (!op.parse(argc, argv)||op.exist("help")){
    std::cout<<op.error_full()<<op.usage();
    return 0;
  }



  // input
  Graph Gin;
  
  if(op.exist("input")) Gin = Graph(op.get<char *>("input"));
  else Gin = Graph("../instances/exact/exact001.gr");

  Gin.show();
  Gin.merge_nodes(0,9);
  Gin.show();
  
  return 0;
  
}
