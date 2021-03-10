#include <vector>
#include <iostream>
#include <algorithm>

#include "main.h"
#include "graph.h"

using namespace std;

//search clique in G
vector<int> search_clique(Graph& G, int u) {
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
            if(i != j) {
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
int check_unaffordable(Graph& G, int u, int v, int& obj) {
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
    }else {
        return 0;
    }
    return 0;
}

/*
graph reduction
*/
int reduction(Graph& G, const Graph& G_orig, int obj, vector <edge>& sol) {
  const int N = G.num_nodes;

  for(int u=0; u < N; u++) {
      vector<int> clique = search_clique(G, u);
      if(clique.size() > 0) {
        G.delete_nodes(clique, sol, G);
        return 1;
      }
      for(int v=u+1; v < N; v++) {
          int cost = check_unaffordable(G, u, v, obj);
          if(cost > 0) {
              G.permanent(u, v, sol, G);
              G.merge_nodes(u, v, sol, G);
              obj -= cost;
              return 1;
          }
      }
  }
  return 0;
}

void cal_reduction(Graph& G, const Graph& G_orig, int obj, vector <edge>& sol) {
    int flag = 0;
    do {
        flag = reduction(G, G_orig, obj, sol);
    } while(flag == 1);
}

vector<int> open_adjacency_set(Graph& G, int v) {
    int N = G.num_nodes;
    vector<int> N_v;

    for(int i=0; i < N; i++) {
        if(G.weight[v][i] > 1) N_v.push_back(G.weight[v][i]);
    }
    return N_v;
}

int delta_v(Graph& G, vector<int> adj_v) {
    int N = G.num_nodes;
    int delta_cost = 0;
    for(int i=0; i < N; i++) {
        for(int j=i+1; j < N; j++) {
            if(G.weight[i][j] <= 0) delta_cost += G.weight[i][j];
        }
    }
    return delta_cost;
}

int gamma_v(Graph& G, vector<int> closed_adj_set) {
    int gamma_cost = 0;
    int N = G.num_nodes;
    vector<int> diff;
    set_difference(G.node_names.begin(), G.node_names.end(), closed_adj_set.begin(), closed_adj_set.end(), inserter(diff, diff.end()));
    for(auto& s: diff) {
        for(auto& t: closed_adj_set) {
            if(G.weight[s][t] > 0) gamma_cost += G.weight[s][t];
        }
    }
    return gamma_cost;
}

vector<int> edge_from_x_to_openNv(Graph& G, int x, int v) {
    vector<int> N_v;
    vector<int> edge_to_x;
    N_v = open_adjacency_set(G, v);
    for(int i=0; i < N_v.size(); i++) {
        if(G.weight[x][i] > 0)  edge_to_x.push_back(i);
    }
    return edge_to_x;
}

void make_clique(Graph& G, vector<int> closed_N_v, int obj, vector <edge>& sol) {
    for(int i=0; i < closed_N_v.size(); i++) {
        for(int j=i+1; j < closed_N_v.size(); j++) {
            if(G.weight[i][j] < 0) {
                G.weight[i][j] = 1;
                obj += 1;
            }
        }
    }
}

vector<int> adj_adj_set(Graph& G, vector<int> closed_adj_set) {
    int N = G.num_nodes;
    vector<int> adj_Nv;
    for(int i=0; i < N; i++) {
        for(int j=0; j < closed_adj_set.size(); j++) {
            bool in = any_of(closed_adj_set.begin(), closed_adj_set.end(), [&j](int x){return x == j;});
            if(in == false) {
                if(G.weight[i][j] > 0) adj_Nv.push_back(i);
            }
        }
    }
    return adj_Nv;
}

void kernelization_EdgeCuts(Graph& G, const Graph& G_orig, int obj, vector <edge>& sol) {
    vector<int> adj_set;
    int delta;
    int gamma;
    int N = G_orig.num_nodes;
    for(int v=0; v < N; v++) {
        adj_set = open_adjacency_set(G, v);
        adj_set.push_back(v);
        delta = delta_v(G, adj_set);
        gamma = gamma_v(G, adj_set);
        //Step 1
        if(2*delta + gamma < adj_set.size()) {
            make_clique(G, adj_set, obj, sol);
            //Step 2
            if(2*delta + gamma < adj_set.size()) {
                vector<int> adj_Nv;
                adj_Nv = adj_adj_set(G, adj_set);
                for(auto& x: adj_Nv) {
                    vector<int> edge;
                    edge = edge_from_x_to_openNv(G,x,v);
                    int edge_cost = 0;
                    for(auto& e: edge) {
                        edge_cost += G.weight[x][e];
                    }
                    if(edge_cost <= adj_set.size() / 2) {
                        //delete E(x, N(v))
                    }
                }
                //Step 3
                if(2*delta + gamma < adj_set.size()) {
                    adj_set = open_adjacency_set(G, v);
                    adj_set.push_back(v);
                    adj_Nv = adj_adj_set(G, adj_set);
                    if(adj_Nv.size() > 0) {
                        // merge N[v] into a single vertex v'
                    }
                }
            }
        }

    }
}