#include <vector>
#include <iostream>
#include <algorithm>
#include "random.hpp"

#include "main.h"
#include "graph.h"
#include "solver.h"

using namespace std;


int naive_branching(const Graph& G, const Graph& G_orig,int max_obj, vector <edge>& sol){

  if(G.num_nodes <= 1) return 0;

  if(G.num_nodes == 2){
    Graph Gnew = G;
    if(Gnew.weight[0][1] > 0 && Gnew.flag[0][1] == 0) Gnew.permanent(0, 1, sol, G_orig);
    if(Gnew.weight[0][1] <= 0 && Gnew.flag[0][1] == 0) Gnew.forbid(0, 1, sol, G_orig);
    return 0;
  }

  vector <int> triple;

  if(!G.conflict_triple(triple)){
    Graph Gnew = G;
    FOR(u, 0, G.num_nodes-1) FOR(v, u+1, G.num_nodes){
      if(G.weight[u][v] > 0 && G.flag[u][v] == 0) Gnew.permanent(u, v, sol, G_orig);
      if(G.weight[u][v] <= 0 && G.flag[u][v] == 0) Gnew.forbid(u, v, sol, G_orig);
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
    if(Gnext.flag[v][w] == 0) Gnext.permanent(v, w, best_sol, G_orig);
    if(Gnext.flag[u][v] == 0) Gnext.permanent(u, v, best_sol, G_orig);
    if(Gnext.flag[u][w] == 0) Gnext.permanent(u, w, best_sol, G_orig);
    int tmp = Gnext.merge_nodes(b, c, best_sol, G_orig);

    if(tmp == -1) cerr << "err" << endl;
    t += tmp;
    tmp = Gnext.merge_nodes(a, b, best_sol, G_orig);

    if(tmp == -1) cerr << "err" << endl;
    t += tmp;

    int tt = naive_branching(Gnext, G_orig, max_obj+G.weight[v][w], best_sol);
    if (tt != -1){
      best = tt + t - G.weight[v][w];
      if(best < max_obj) max_obj = best;
    }
  }

  // merge u & v, and forbid u & w
  if(G.flag[u][w] != 1){
    Graph Gnext = G;
    vector <edge> tmp_sol;

    if(G.flag[u][w] == 0) Gnext.forbid(u,w, tmp_sol, G_orig);
    if(G.flag[v][w] == 0) Gnext.forbid(v,w, tmp_sol, G_orig);
    if(G.flag[u][v] == 0) Gnext.permanent(u,v, tmp_sol, G_orig);
    int t = Gnext.merge_nodes(u,v, tmp_sol, G_orig);
    if(t == -1) cerr << "err" << endl;


    int tt = naive_branching(Gnext, G_orig, max_obj-G.weight[u][w], tmp_sol);
    if (tt != -1 && (best == -1 || best > tt + G.weight[u][w])){
      best = tt + G.weight[u][w];
      if(best < max_obj) max_obj = best;
      best_sol.clear();
      best_sol = tmp_sol;
    }
  }

  // forbid u & v 
  if(G.flag[u][v] != 1){
    Graph Gnext = G;
    vector <edge> tmp_sol;
    Gnext.forbid(u,v, tmp_sol, G_orig);
    
    int tt = naive_branching(Gnext, G_orig, max_obj-G.weight[u][v], tmp_sol);
    if (tt != -1 && (best == -1 || best > tt + G.weight[u][v])){
      best = tt + G.weight[u][v];
      best_sol.clear();
      best_sol = tmp_sol;
    }
  }

  if(best != -1){
    sol.insert(sol.end(), best_sol.begin(), best_sol.end());
  }

  return best;
}


int random_pivot(Graph& G, const Graph& G_orig, std::vector <edge>& sol){
  rnd.seed(RANDOM_SEED);
  int n = G.num_nodes;
  vector <int> nodes;
  REP(i, n) nodes.push_back(i);

  int cost = 0;

  while(n > 1){
    int k = rnd(0, n-1);
    int pivot = nodes[k];
    vector <int> neighbor;
    nodes.erase(nodes.begin()+k);
    n--;

    for(int i=n-1; i >= 0; i--){
      if(G.weight[nodes[i]][pivot] > 0) {
        neighbor.push_back(nodes[i]);
        if(G.flag[nodes[i]][pivot] == 0) G.permanent(nodes[i],pivot,sol,G_orig);
        nodes.erase(nodes.begin()+i);
      }
    }
    n -= neighbor.size();

    if(neighbor.size() > 1){
      FOR(i,0, (int) neighbor.size()-1) FOR(j, i+1, (int) neighbor.size()){
        if(G.flag[neighbor[i]][neighbor[j]] == 0) G.permanent(neighbor[i],neighbor[j],sol,G_orig);

        if(G.weight[neighbor[i]][neighbor[j]] < 0){
          cost-= G.weight[neighbor[i]][neighbor[j]];
          G.add_edge(neighbor[i], neighbor[j]);
        }
      }
    }
    if(neighbor.size() > 0 && n > 0){
      REP(i, (int) neighbor.size()) REP(j, n){
        if(G.flag[neighbor[i]][nodes[j]] == 0) G.forbid(neighbor[i],nodes[j],sol,G_orig);
        
        if(G.weight[neighbor[i]][nodes[j]] > 0){
          cost += G.weight[neighbor[i]][nodes[j]];
          G.delete_edge(neighbor[i],nodes[j]);
        }
      }
    }
  }
  
  return cost;
}
