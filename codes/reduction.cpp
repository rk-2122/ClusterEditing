#include <vector>
#include <iostream>
#include <algorithm>

#include "main.h"
#include "graph.h"
#include "reduction.h"

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
        if(u != i && G.weight[u][i] > 0) clique.push_back(i);
    }

    if(clique.size() == 1) return;

    for(int i=0; i < clique.size(); i++) {
        for(int j=0; j < n; j++) {
            if(clique[i] == j || u == j)  continue;

            if(G.weight[u][j] > 0) {
                if(G.weight[clique[i]][j] <= 0){ 
                    clique.clear();
                    return;
                }
            }else {
                if(G.weight[clique[i]][j] > 0){ 
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
        if(G.weight[u][w] > 0 && G.weight[v][w] > 0) {
            sum_icf += min(G.weight[u][w], G.weight[v][w]);
        }
        else if(G.weight[u][w] > 0 || G.weight[v][w] > 0) {
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

    return 0;
}

/*
graph reduction
*/
int reduction(Graph& G, const Graph& G_orig, const int obj, vector <edge>& sol) {
    int N = G.num_nodes;

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
    int flag = 0;
    do {
        cost += flag;
        flag = reduction(G, G_orig, obj, sol);    
    } while(flag != -1);

    return cost;
}