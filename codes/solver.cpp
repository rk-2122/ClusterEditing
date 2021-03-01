#include <vector>
#include <iostream>
#include <algorithm>

#include "main.h"
#include "graph.h"
#include "solver.h"

using namespace std;


int naive_branching(Graph& G, const Graph& G_orig,int max_obj, vector <edge>& sol){

  if(G.num_nodes <= 1){
    return 0;
  }
  if(G.num_nodes == 2){
    if(G.weight[0][1] > 0 && G.flag[0][1] == 0) G.permanent(0, 1, sol, G_orig);
    if(G.weight[0][1] <= 0 && G.flag[0][1] == 0) G.forbid(0, 1, sol, G_orig);
    return 0;
  }

  vector <int> triple;

  if(!G.conflict_triple(triple)){
    FOR(u, 0, G.num_nodes-1) FOR(v, u+1, G.num_nodes){
      if(G.weight[u][v] > 0 && G.flag[u][v] == 0) G.permanent(u,v,sol,G_orig);
      if(G.weight[u][v] <= 0 && G.flag[u][v] == 0) G.forbid(u, v, sol, G_orig);
    }
    return 0;
  }

  if(max_obj == 0) return -1;

  int best = -1;
  vector <edge> best_sol;

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

    int t = 0;
    t += Gnext.merge_nodes(b, c, best_sol, G_orig);
    t += Gnext.merge_nodes(a, b, best_sol, G_orig);
    cout << "a: " << max_obj<< endl;
    vector <edge> tmp_sol;
    int tt = naive_branching(Gnext, G_orig, max_obj+G.weight[v][w], tmp_sol);
    if (tt != -1){
      best = tt + t - G.weight[v][w];
      best_sol = tmp_sol;
    }
  }

  // merge u & v, and forbid u & w
  Graph Gnext = G;
  vector <edge> tmp_sol;
  int t = G.weight[u][w];
  Gnext.forbid(u,w, tmp_sol, G_orig);
  Gnext.delete_edge(u,w);
  t += Gnext.merge_nodes(u,v, tmp_sol, G_orig);
  cout << "b: " << max_obj<< endl;
  int tt = naive_branching(Gnext, G_orig, max_obj-t, tmp_sol);
  if (tt != -1 && best > t + tt){
    best = t+tt;
    best_sol.clear();
    best_sol = tmp_sol;
    //best_sol's update
  }
  // forbid u & v and u & w and v & w
  Gnext = G;
  tmp_sol.clear();
  t = G.weight[u][v] + G.weight[u][w];
  Gnext.forbid(u,w, tmp_sol,G_orig);
  Gnext.forbid(u,v, tmp_sol, G_orig);
  if(G.flag[v][w] == 0) Gnext.forbid(v,w, tmp_sol, G_orig);
  Gnext.delete_edge(u,w);
  Gnext.delete_edge(v,u);
  cout << "c: " << max_obj<< endl;
  tt = naive_branching(Gnext, G_orig, max_obj-t, sol);
  if (tt != -1 && best > t + tt){
    best = t+tt;
    best_sol.clear();
    best_sol = tmp_sol;
  }

  if(best != -1){
    sol.insert(sol.end(), best_sol.begin(), best_sol.end());
  }

  return best;
}
