#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <time.h>

#include "cmdline.h"

#include "main.h"
#include "graph.h"
#include "solver.h"


using namespace std;


int main(int argc, char *argv[]){
  // parser
  cmdline::parser op;
  op.add<string>("input", 'i', "input file name", false);
  op.add<string>("output", 'o', "output fine name", false);
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

  vector <edge> sol;

  /*
  int t = random_pivot(Gin, Gin, sol);
  cout << t << endl;
  show_sol(sol);
 
  return 0;
  */
  
  clock_t c_start = clock();
  int obj = naive_branching(Gin, Gin, 100, sol);
  clock_t c_end = clock();

  if(DEBUG){
    cout << op.get<string>("input") << " "<< obj << " "<< (double) (c_end - c_start)/ CLOCKS_PER_SEC << endl;
  }

  if(op.exist("output")){
    const char *fo = op.get<string>("output").c_str();
    write_sol(sol,fo);
  }

  return 0;
}


void show_sol(const vector <edge>& sol){
  for (auto u: sol){
    cout << u.first+1 << " " << u.second+1 << endl;
  }

  return;
}

void write_sol(const vector <edge>& sol, const char *fo){
  ofstream ot(fo);

  for (auto u: sol){
    ot << u.first+1 << " " << u.second+1 << endl;
  }
  ot.close();
}