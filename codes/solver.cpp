#include <vector>
#include <list>
#include <iostream>
#include <algorithm>
#include "random.hpp"

#include "main.h"
#include "graph.h"
#include "solver.h"

using namespace std;


int naive_branching(Graph& G, const Graph& G_orig,int max_obj, vector <edge>& sol){

  if(G.num_nodes <= 1) return 0;

  if(G.num_nodes == 2){
    if(G.Weight(0,1) > 0 && G.Flag(0,1) == 0){
      G.permanent(0, 1, sol, G_orig); 
      G.reset_flag(0,1);
    }
    if(G.Weight(0,1) <= 0 && G.Flag(0,1) == 0){
      G.forbid(0, 1, sol, G_orig);
      G.reset_flag(0,1);
    }
    
    return 0;
  }

  vector <int> triple;

  if(!G.conflict_triple(triple)){
    FOR(u, 0, G.num_nodes-1) FOR(v, u+1, G.num_nodes){
      if(G.Weight(u,v) > 0 && G.Flag(u,v) == 0){
        G.permanent(u, v, sol, G_orig);
        G.reset_flag(u,v);
      }
      if(G.Weight(u,v) <= 0 && G.Flag(u,v) == 0){
        G.forbid(u, v, sol, G_orig);
        G.reset_flag(u,v);
      }
      
    }
    return 0;
  }

  if(max_obj <= 0) return -1;

  
  // for naive branching
  int a_name = G.node_names[triple[0]];
  int b_name = G.node_names[triple[1]];
  int c_name = G.node_names[triple[2]];


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

      int u_name = G.node_names[u];
      int v_name = G.node_names[v];
      if(u_name > v_name) SWAP(int, u_name, v_name);

      int w = G.Weight(u,v);
      G.forbid(u, v, best_sol, G_orig);
      G.flip_edge(u,v);
      
      best = naive_branching(G, G_orig, max_obj-w, best_sol);
      if(best != -1) best += w;

      G.reset_flag(G.name_to_ind[u_name],G.name_to_ind[v_name]);
      G.flip_edge(G.name_to_ind[u_name],G.name_to_ind[v_name]);

      if(merge_flag){
        vector <edge> tmpsol;
        G.permanent(G.name_to_ind[u_name], G.name_to_ind[v_name], tmpsol, G_orig);
        MergeData mg(G_orig.num_nodes);
        int mergecost = G.merge_nodes(G.name_to_ind[u_name], G.name_to_ind[v_name], tmpsol, mg, G_orig);
        int cost = naive_branching(G, G_orig, max_obj-mergecost, tmpsol);
        if((cost != -1) && (best == -1 || cost + mergecost < best)){
          best = cost + mergecost;
          best_sol = tmpsol;
        }
       
        G.expand_nodes(u_name, v_name, mg);
        G.reset_flag(G.name_to_ind[u_name],G.num_nodes-1);
      }

      if(best != -1) sol.insert(sol.end(), best_sol.begin(), best_sol.end());
      return best;
    }
  }

  // merge a, b, & c
  int a = G.name_to_ind[a_name];
  int b = G.name_to_ind[b_name];
  int c = G.name_to_ind[c_name];

  int w = G.Weight(b,c);

  if(G.Flag(b,c) != -1){
    int t = 0;
    bool flag_ab = false, flag_ac = false, flag_bc = false;
    if(G.Flag(a,b) == 0){
      G.permanent(a, b, best_sol, G_orig);
      flag_ab = true;
    }
    if(G.Flag(a,c) == 0){
      G.permanent(a, c, best_sol, G_orig);
      flag_ac = true;
    }
    if(G.Flag(b,c) == 0){
      G.permanent(b, c, best_sol, G_orig);
      flag_bc = true;
    }
    
    G.weight[b_name][c_name]--; // for when G.weight[b_name][c_name] == 0
    G.weight[c_name][b_name]--; // for when G.weight[b_name][c_name] == 0
    G.flip_edge(b,c);

    if (G.node_names[a] > G.node_names[b]) SWAP(int, a, b);
    if (G.node_names[b] > G.node_names[c]) SWAP(int, b, c);
    if (G.node_names[a] > G.node_names[b]) SWAP(int, a, b);

    MergeData mg_bc(G_orig.num_nodes);
    int tmp = G.merge_nodes(b, c, best_sol, mg_bc, G_orig);
    
    if(tmp == -1) cerr << "err" << endl;
    t += tmp;
    if(b > c) b--;
    if(a > c) a--;
    
    MergeData mg_ab(G_orig.num_nodes);
    tmp = G.merge_nodes(a, b, best_sol, mg_ab, G_orig);
    if(tmp == -1) cerr << "err" << endl;
    t += tmp;
  
    int tt = naive_branching(G, G_orig, max_obj+w, best_sol);
    if (tt != -1){
      best = tt + t - w;
      if(best < max_obj) max_obj = best;
    }

    int a_name_srt = a_name; 
    int b_name_srt = b_name; 
    int c_name_srt = c_name;
    if (a_name_srt > b_name_srt) SWAP(int, a_name_srt, b_name_srt);
    if (b_name_srt > c_name_srt) SWAP(int, b_name_srt, c_name_srt);
    if (a_name_srt > b_name_srt) SWAP(int, a_name_srt, b_name_srt);
    G.expand_nodes(a_name_srt,b_name_srt,mg_ab);
    G.expand_nodes(b_name_srt,c_name_srt,mg_bc);

    a = G.name_to_ind[a_name];
    b = G.name_to_ind[b_name];
    c = G.name_to_ind[c_name];

    G.flip_edge(b,c);
    G.weight[b_name][c_name]++;
    G.weight[c_name][b_name]++;

    if(flag_ab) G.reset_flag(a,b);
    if(flag_bc) G.reset_flag(b,c);
    if(flag_ac) G.reset_flag(a,c);
    
  }

  // merge a & b, and forbid a & c and b & c
  if(G.Flag(a,c) != 1){
    vector <edge> tmp_sol;
    bool flag_ac = false, flag_bc = false, flag_ab = false;
    if(G.Flag(a,c) == 0){
      G.forbid(a,c, tmp_sol, G_orig);
      flag_ac = true;
    }
    if(G.Flag(b,c) == 0){
      G.forbid(b,c, tmp_sol, G_orig);
      flag_bc = true;
    }
    if(G.Flag(a,b) == 0){
      G.permanent(a,b, tmp_sol, G_orig);
      flag_ab = true;
    }
    int w = G.Weight(a,c);
    G.flip_edge(a,c);

    MergeData mg_ab(G_orig.num_nodes);
    int t = G.merge_nodes(a,b, tmp_sol, mg_ab, G_orig);
    if(t == -1) cerr << "err" << endl;

    int tt = naive_branching(G, G_orig, max_obj-w, tmp_sol);
    if (tt != -1 && (best == -1 || best > t+ tt + w)){
      best = t + tt + w;
      if(best < max_obj) max_obj = best;
      best_sol.clear();
      best_sol = tmp_sol;
    }
    G.expand_nodes(a_name,b_name,mg_ab);

    a = G.name_to_ind[a_name];
    b = G.name_to_ind[b_name];
    c = G.name_to_ind[c_name];

    G.flip_edge(a,c);
    if(flag_ab) G.reset_flag(a,b);
    if(flag_bc) G.reset_flag(b,c);
    if(flag_ac) G.reset_flag(a,c);
  }

  // forbid a & b 
  if(G.Flag(a,b) != 1){
    vector <edge> tmp_sol;

    G.forbid(a,b, tmp_sol, G_orig);
    int w = G.Weight(a,b);
    G.flip_edge(a,b);
    
    int tt = naive_branching(G, G_orig, max_obj-w, tmp_sol);
    if (tt != -1 && (best == -1 || best > tt + w)){
      best = tt + w;
      best_sol.clear();
      best_sol = tmp_sol;
    }
    REP(i, G.num_nodes){
      if(G.node_names[i] == a_name) a = i;
      else if(G.node_names[i] == b_name) b = i;
    }
    G.flip_edge(a,b);
    G.reset_flag(a,b);
  }

  if(best != -1){
    sol.insert(sol.end(), best_sol.begin(), best_sol.end());
  }

  return best;
}

int random_pivot(const Graph& G, const Graph& G_orig, std::vector <edge>& sol){
  Graph Gcopy = G;
  rnd.seed(RANDOM_SEED);

  int cost = 0;

  while(Gcopy.num_nodes > 1){
    int k = rnd(0, Gcopy.num_nodes-1);
    int pivot = Gcopy.node_names[k];
    vector <int> N;
    N.push_back(k);
    
    for(int i: Gcopy.neighbors[pivot]) N.push_back(Gcopy.name_to_ind[i]);

    if(Gcopy.neighbors[pivot].size() > 1){
      for(auto u = Gcopy.neighbors[pivot].begin(); next(u) != Gcopy.neighbors[pivot].end(); u++){ 
        for(auto v = next(u); v != Gcopy.neighbors[pivot].end(); v++){
          if(Gcopy.weight[*u][*v] < 0){
            cost-= Gcopy.weight[*u][*v];
            Gcopy.add_edge(Gcopy.name_to_ind[*u], Gcopy.name_to_ind[*v]);
          }
        }
      }
    }
    if(Gcopy.neighbors[pivot].size() > 0 && Gcopy.num_nodes - Gcopy.neighbors[pivot].size() > 1){
      for(int i : Gcopy.neighbors[pivot]){
        REP(j, Gcopy.num_nodes){
          if(j != k && Gcopy.Weight(j,k) <= 0 && Gcopy.Weight(Gcopy.name_to_ind[i],j) > 0){
            cost += Gcopy.Weight(Gcopy.name_to_ind[i],j);
            Gcopy.delete_edge(Gcopy.name_to_ind[i],j);
          }
        }
      }
    }
    Gcopy.delete_nodes(N, sol, G_orig);
  }
  
  return cost;
}
