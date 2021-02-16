#include <vector>

#include "main.h"
#include "graph.h"

using namespace std;


void reduction(Graph& G, int& obj) {

  return;
}



int naive_branching(const Graph& G, int max_obj){

  if(G.num_nodes <= 2) return 0;

  vector <int> triple;
  
  if(!G.conflict_triple(triple)) return 0;
  
  if(max_obj == 0) return -1;

  int best = -1;
  
  int u = triple[0];
  int v = triple[1];
  int w = triple[2];
    
  Graph Gnext = G;

  // merge u, v, & w
  if(G.flag[v][w] != -1){
    int a = u, b = v, c = w;
    if (a > b) SWAP(int, a, b);
    if (b > c) SWAP(int, b, c);
    if (a > b) SWAP(int, a, b);
      
    Gnext.merge_nodes(b,c);
    Gnext.merge_nodes(a,b);
    best = naive_branching(Gnext, max_obj-1);
  }

  // merge u & v, and forbid u & w

  // forbid u & v and u & w and v & w

  return true;
}
