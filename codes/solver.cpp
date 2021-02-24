#include <vector>
#include <iostream>
#include <algorithm>

#include "main.h"
#include "graph.h"

using namespace std;


void reduction(Graph& G, int& obj) {
  int N = G.num_nodes;

  //Rule 2
  int icf[N][N];
  int icp[N][N];

  for(int u=0; u < N; u++) {
    for(int v=u+1; v < N; v++) {
      vector<int> N_u;
      vector<int> N_v;
      vector<int> N_uv;
      vector<int> delta_uv;
      vector<int> union_uv;
      int sum_icf = 0;
      int sum_icp = 0;

      for(int i=0; i < N; i++) {
        if(u != i) {
          if(G.weight[u][i] > 0) N_u.push_back(i);
        }

        if(v != i) {
          if(G.weight[v][i] > 0) N_v.push_back(i);
        }
      }

      sort(N_u.begin(), N_u.end());
      sort(N_v.begin(), N_v.end());
      set_union(begin(N_u), end(N_u), begin(N_v), end(N_v), inserter(union_uv, end(union_uv)));
      set_intersection(begin(N_u), end(N_u), begin(N_v), end(N_v), inserter(N_uv, end(N_uv)));
      set_difference(begin(N_uv), end(N_uv), begin(union_uv), end(union_uv), inserter(delta_uv, end(delta_uv)));

      for(int i=0; i < N_uv.size(); i++) {
        int w = N_uv[i];
        sum_icf += min(G.weight[u][w], G.weight[v][w]);
      }

      for(int i=0; i < delta_uv.size(); i++) {
        int w = delta_uv[i];
        sum_icp += min(abs(G.weight[u][w]), abs(G.weight[v][w]));
      }

      icf[u][v] = max(0, G.weight[u][v]) + sum_icf;
      icf[v][u] = max(0, G.weight[u][v]) + sum_icf;

      icp[u][v] = max(0, -G.weight[u][v]) + sum_icp;
      icp[u][v] = max(0, -G.weight[u][v]) + sum_icp;
    }
  }

  for(int u=0; u < N; u++) {
    for(int v=u+1; v < N; v++) {
      if(icf[u][v] > obj && icp[u][v]) {
        cout << "not solvable" << endl;
        return ;
      }else if(icf[u][v] > obj) {
        G.weight[u][v] = 1;
        G.weight[v][u] = 1;
        G.permanent(u, v);
      }else if(icp[u][v] > obj) {
        G.weight[u][v] = -1;
        G.weight[v][u] = -1;
        G.forbid(u, v);
      }
    }
  }

  //merge処理. 削除した頂点に関する情報はまだ未実装.
  for(int u=0; u < N; u++) {
    for(int v=u+1; v < N; v++) {
      if(G.flag[u][v] == 1) {
        G.merge_nodes(u, v);
        obj -= icp[u][v];
      }
    }
  }
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
