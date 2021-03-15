#include <vector>
#include <list>
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
    if(Gnew.Weight(0,1) > 0 && Gnew.Flag(0,1) == 0) Gnew.permanent(0, 1, sol, G_orig);
    if(Gnew.Weight(0,1) <= 0 && Gnew.Flag(0,1) == 0) Gnew.forbid(0, 1, sol, G_orig);
    return 0;
  }

  vector <int> triple;

  if(!G.conflict_triple(triple)){
    Graph Gnew = G;
    FOR(u, 0, G.num_nodes-1) FOR(v, u+1, G.num_nodes){
      if(G.Weight(u,v) > 0 && G.Flag(u,v) == 0) Gnew.permanent(u, v, sol, G_orig);
      if(G.Weight(u,v) <= 0 && G.Flag(u,v) == 0) Gnew.forbid(u, v, sol, G_orig);
    }
    return 0;
  }

  if(max_obj <= 0) return -1;

  
  int best = -1;
  vector <edge> best_sol;

  // edge branching
  FOR(u, 0, G.num_nodes-1) FOR(v, u+1, G.num_nodes){
    if(G.Weight(u,v) <= 0 || G.Flag(u,v) != 0) continue;
    
    int cnt = 0;
    bool merge_flag = true;
    FOR(z, 0, G.num_nodes){
      if(u == z || v == z) continue;
      if((G.Weight(u,z) > 0 && G.Weight(v,z) <= 0) || (G.Weight(u,z) <= 0 && G.Weight(v,z) > 0)) cnt++;
      if((G.Flag(u,z) == -1 && G.Flag(v,z) == 1) || (G.Flag(u,z) == 1 && G.Flag(v,z) == -1)) merge_flag = false;
    }

    if(cnt >= 3){
      Graph Gtmp = G;
      Gtmp.forbid(u, v, best_sol, G_orig);
      Gtmp.flip_edge(u,v);
      best = naive_branching(Gtmp, G_orig, max_obj-G.Weight(u,v), best_sol);
      if(best != -1){
        best += G.Weight(u,v);
      }

      if(merge_flag){
        Gtmp.reset_flag(u,v);
        Gtmp.flip_edge(u,v);
        vector <edge> tmpsol;
        Gtmp.permanent(u, v, tmpsol, G_orig);
        MergeData mg(G_orig.num_nodes);
        int mergecost = Gtmp.merge_nodes(u, v, tmpsol, mg, G_orig);
        int cost = naive_branching(Gtmp, G_orig, max_obj-mergecost, tmpsol);
        if((cost != -1) && (best == -1 || cost + mergecost < best)){
          best = cost + mergecost;
          best_sol = tmpsol;
        }
      }
      if(best != -1) sol.insert(sol.end(), best_sol.begin(), best_sol.end());
      return best;
    }
  }

  // naive branching
  int u = triple[0];
  int v = triple[1];
  int w = triple[2];

  // merge u, v, & w
  if(G.Flag(v,w) != -1){
    Graph Gnext = G;

    int t = 0;
    if(Gnext.Flag(v,w) == 0) Gnext.permanent(v, w, best_sol, G_orig);
    if(Gnext.Flag(u,v) == 0) Gnext.permanent(u, v, best_sol, G_orig);
    if(Gnext.Flag(u,w) == 0) Gnext.permanent(u, w, best_sol, G_orig);
    Gnext.flip_edge(v,w);

    int a = u, b = v, c = w;
    if (a > b) SWAP(int, a, b);
    if (b > c) SWAP(int, b, c);
    if (a > b) SWAP(int, a, b);
  
    MergeData mg_bc(G_orig.num_nodes);
    int tmp = Gnext.merge_nodes(b, c, best_sol, mg_bc, G_orig);
    if(tmp == -1) cerr << "err" << endl;
    t += tmp;

    MergeData mg_ab(G_orig.num_nodes);
    tmp = Gnext.merge_nodes(a, b, best_sol, mg_ab, G_orig);
    if(tmp == -1) cerr << "err" << endl;
    t += tmp;

    int tt = naive_branching(Gnext, G_orig, max_obj+G.Weight(v,w), best_sol);
    if (tt != -1){
      best = tt + t - G.Weight(v,w);
      if(best < max_obj) max_obj = best;
    }
  }

  // merge u & v, and forbid u & w and v & w
  if(G.Flag(u,w) != 1){
    Graph Gnext = G;
    vector <edge> tmp_sol;

    if(G.Flag(u,w) == 0) Gnext.forbid(u,w, tmp_sol, G_orig);
    if(G.Flag(v,w) == 0) Gnext.forbid(v,w, tmp_sol, G_orig);
    if(G.Flag(u,v) == 0) Gnext.permanent(u,v, tmp_sol, G_orig);
    Gnext.flip_edge(u,w);
    
    MergeData mg_uv(G_orig.num_nodes);
    int t = Gnext.merge_nodes(u,v, tmp_sol, mg_uv, G_orig);
    if(t == -1) cerr << "err" << endl;

    int tt = naive_branching(Gnext, G_orig, max_obj-G.Weight(u,w), tmp_sol);
    if (tt != -1 && (best == -1 || best > t+ tt + G.Weight(u,w))){
      best = t + tt + G.Weight(u,w);
      if(best < max_obj) max_obj = best;
      best_sol.clear();
      best_sol = tmp_sol;
    }
  }

  // forbid u & v 
  if(G.Flag(u,v) != 1){
    Graph Gnext = G;
    vector <edge> tmp_sol;
    Gnext.forbid(u,v, tmp_sol, G_orig);
    Gnext.flip_edge(u,v);
    
    int tt = naive_branching(Gnext, G_orig, max_obj-G.Weight(u,v), tmp_sol);
    if (tt != -1 && (best == -1 || best > tt + G.Weight(u,v))){
      best = tt + G.Weight(u,v);
      best_sol.clear();
      best_sol = tmp_sol;
    }
  }

  if(best != -1){
    sol.insert(sol.end(), best_sol.begin(), best_sol.end());
  }

  return best;
}


int random_pivot(const Graph& G, const Graph& G_orig, std::vector <edge>& sol){
  Graph Gnew = G;
  rnd.seed(RANDOM_SEED);
  int n = Gnew.num_nodes;
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
      if(Gnew.Weight(nodes[i],pivot) > 0) {
        neighbor.push_back(nodes[i]);
        if(Gnew.Flag(nodes[i],pivot) == 0) Gnew.permanent(nodes[i],pivot,sol,G_orig);
        nodes.erase(nodes.begin()+i);
      }
    }
    n -= neighbor.size();

    if(neighbor.size() > 1){
      FOR(i,0, (int) neighbor.size()-1) FOR(j, i+1, (int) neighbor.size()){
        if(Gnew.Flag(neighbor[i],neighbor[j]) == 0) Gnew.permanent(neighbor[i],neighbor[j],sol,G_orig);

        if(Gnew.Weight(neighbor[i],neighbor[j]) < 0){
          cost-= Gnew.Weight(neighbor[i],neighbor[j]);
          Gnew.add_edge(neighbor[i], neighbor[j]);
        }
      }
    }
    if(neighbor.size() > 0 && n > 0){
      REP(i, (int) neighbor.size()) REP(j, n){
        if(Gnew.Flag(neighbor[i],nodes[j]) == 0) Gnew.forbid(neighbor[i],nodes[j],sol,G_orig);
        
        if(Gnew.Weight(neighbor[i],nodes[j]) > 0){
          cost += Gnew.Weight(neighbor[i],nodes[j]);
          Gnew.delete_edge(neighbor[i],nodes[j]);
        }
      }
    }
  }
  
  return cost;
}
