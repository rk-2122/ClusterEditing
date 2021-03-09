#include <vector>
#include <iostream>
#include <algorithm>

#include "main.h"
#include "graph.h"
#include "reduction.h"

using namespace std;

//search clique in G
vector<int> search_clique(const Graph& G, const int u) {
    int N = G.num_nodes;
    vector<int> clique;
    vector<int> N_u;

    N_u.push_back(u);

    //search neighbor
    for(int i=0; i < N; i++) {
        if(u != i) {
            if(G.weight[u][i] > 0) N_u.push_back(i);
        }
    }

    for(int i=0; i < N_u.size(); i++) {
        for(int j=0; j < N; j++) {
            if(N_u[i] != j) {
                bool in = any_of(N_u.begin(), N_u.end(), [&j](int x){return x == j;});
                if(in == true) {
                    if(G.weight[i][j] <= 0) {
                        vector<int> empty;
                        return empty;
                    }
                }else {
                    if(G.weight[i][j] > 0) {
                        vector<int> empty;
                        return empty;
                    }
                }
            }
        }
    clique.push_back(i);
    }
    return clique;
}

/*
set permanent or forbidden for each u, v
*/
int check_unaffordable(const Graph& G, const int u, const int v, const int obj) {
    int N = G.num_nodes;
    int icf;
    int icp;
    int sum_icf = 0;
    int sum_icp = 0;

    for(int w=0; w < N; w++) {
        if(G.weight[u][w] > 0 && G.weight[v][w] > 0) {
            sum_icf += min(G.weight[u][w], G.weight[v][w]);
        }

        if(G.weight[u][w] > 0 || G.weight[v][w] > 0) {
            sum_icp += min(abs(G.weight[u][w]), abs(G.weight[v][w]));
        }
    }

    icf = max(0, G.weight[u][v]) + sum_icf;
    icp = max(0, -G.weight[u][v]) + sum_icp;

    if(icf > obj && icp > obj) {
        return 0;
    }else if(icf > obj) {
        return icp;
    }else if(icp > obj) {
        return -1;
    }

    return -1;
}

/*
graph reduction
*/
int reduction(Graph& G, const Graph& G_orig, const int obj, vector <edge>& sol) {
  const int N = G.num_nodes;

  for(int u=0; u < N; u++) {
      vector<int> clique = search_clique(G, u);
      if(clique.size() > 0) {
        G.delete_nodes(clique, sol, G);
        return 0;
      }
      for(int v=u+1; v < N; v++) {
          int cost = check_unaffordable(G, u, v, obj);
          if(cost > 0) {
              if (G.flag[u][v] == 0) G.permanent(u, v, sol, G);
              G.merge_nodes(u, v, sol, G);
              return cost;
          }
      }
  }
  return -1;
}

int cal_reduction(Graph& G, const Graph& G_orig, const int obj, vector <edge>& sol) {
    int cost = 0;
    int flag;
    do {
        cost += flag;
        flag = reduction(G, G_orig, obj, sol);        
    } while(flag != -1);

    return cost;
}