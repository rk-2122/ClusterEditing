#include <vector>
#include <iostream>

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
    
  // merge u, v, & w
  if(G.flag[v][w] != -1){
    Graph Gnext = G;
      
    int a = u, b = v, c = w;
    if (a > b) SWAP(int, a, b);
    if (b > c) SWAP(int, b, c);
    if (a > b) SWAP(int, a, b);
      
    Gnext.merge_nodes(b,c);
    Gnext.merge_nodes(a,b);
    cout << "a: " << max_obj<< endl;
    int t = naive_branching(Gnext, max_obj-1);
    if (t != -1) best = t + 1;
  }

  // merge u & v, and forbid u & w
  Graph Gnext = G;
  Gnext.forbid(u,w);
  Gnext.weight[u][w] = -1;
  Gnext.weight[w][u] = -1;
  Gnext.merge_nodes(u,v);
  cout << "b: " << max_obj<< endl;
  int t = naive_branching(Gnext, max_obj-1);
  if (t != -1 && best > t + 1) best = t+1;
  
  // forbid u & v and u & w and v & w
  Gnext = G;
  Gnext.forbid(u,w);
  Gnext.forbid(u,v);
  Gnext.forbid(v,w);
  Gnext.weight[u][w] = -1;
  Gnext.weight[w][u] = -1;
  Gnext.weight[u][v] = -1;
  Gnext.weight[v][u] = -1;
  cout << "c: " << max_obj<< endl;
  t = naive_branching(Gnext, max_obj-2);
  if (t != -1 && best > t + 2) best = t+2;
  
  return best;
}
