#include <vector>
#include <iostream>
#include <algorithm>

#include "main.h"
#include "graph.h"

using namespace std;

void search_clique(const Graph& G, const int u, vector <int>& clique);
int check_unaffordable(const Graph&G, const int u, const int v, const int obj);
int reduction(Graph& G, const Graph& G_orig, const int obj, vector <edge>& sol);

//search clique in G
void search_clique(const Graph& G, const int u, vector<int>& clique) {
    int n = G.num_nodes;

    clique.clear();
    if(n <= 2) return;

    clique.push_back(u);

    //search neighbor
    for(int i=0; i < n; i++) {
        if(u != i && G.Weight(u,i) > 0) clique.push_back(i);
    }

    if(clique.size() == 1) return;

    for(int i=0; i < clique.size(); i++) {
        for(int j=0; j < n; j++) {
            if(clique[i] == j || u == j)  continue;

            if(G.Weight(u,j) > 0) {
                if(G.Weight(clique[i],j) <= 0){ 
                    clique.clear();
                    return;
                }
            }else {
                if(G.Weight(clique[i],j) > 0){ 
                    clique.clear();
                    return;
                }
            }
        }
    }
    return;
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
        if(u == w || v == w) continue;

        if(G.Weight(u,w) > 0 && G.Weight(v,w) > 0) {
            sum_icf += min(G.Weight(u,w), G.Weight(v,w));
        }
        else if(G.Weight(u,w) > 0 || G.Weight(v,w) > 0) {
            sum_icp += min(abs(G.Weight(u,w)), abs(G.Weight(v,w)));
        }
    }

    icf = max(0, G.Weight(u,v)) + sum_icf;
    icp = max(0, -G.Weight(u,v)) + sum_icp;

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
int reduction(Graph& G, const Graph& G_orig, const int obj, vector <edge>& sol) {
    int N = G.num_nodes;
    if(obj < 0) return -1;

    for(int u=0; u < N; u++) {
        vector<int> clique; 
        search_clique(G, u, clique);
        if(clique.size() > 1) {
            G.delete_nodes(clique, sol, G_orig);
            return 0;
        }
        else if(clique.size() == 1){
            G.delete_node(u, sol ,G_orig);
            return 0;
        }
    }

    for(int u=0; u < N-1; u++) {
      for(int v=u+1; v < N; v++) {
          int cost = check_unaffordable(G, u, v, obj);
          if(cost > 0) {
              if (G.Flag(u,v) == 0) G.permanent(u, v, sol, G_orig);
              G.merge_nodes(u, v, sol, G_orig);
              return cost;
          }
      }
    }

    return -1;
}

int cal_reduction(Graph& G, const Graph& G_orig, const int obj, vector <edge>& sol) {
    int cost = 0;
    int flag = 0;
    do {
        cost += flag;
        flag = reduction(G, G_orig, obj-cost, sol);    
    } while(flag != -1 && G.num_nodes >= 3);

    return cost;
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
