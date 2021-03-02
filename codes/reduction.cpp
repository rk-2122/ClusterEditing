#include <vector>
#include <iostream>
#include <algorithm>

#include "main.h"
#include "graph.h"

using namespace std;

//頂点uがグラフGのクリークに含まれるならそのクリークを削除し,　削除した点をdelete_pに保存する.
vector<int> delete_clique(Graph& G, int u) {
    int N = G.num_nodes;
    vector<int> delete_point;
    vector<int> N_u;    //頂点uに隣接する頂点を保存するベクター

    //uの隣接点を探索
    for(int i=0; i < N; i++) {
        if(u != i) {
            if(G.weight[u][i] > 0) N_u.push_back(i);
        }
    }

    delete_point.push_back(u);
    //uの隣接点の個数が0または1ならば, u及びその隣接点がクリークであるので終了. 2個以上なら隣接点がクリークになるか調べる.
    if(N_u.size() < 1) {
        return delete_point;
    }else if(N_u.size() == 1) {
        delete_point.push_back(N_u[0]);
        return delete_point;
    }else {
        while(N_u.size() > 0) {
            int p = N_u.back();
            N_u.pop_back();
            for(int i=0; i < N_u.size(); i++) {
                if(G.weight[p][i] < 0) {
                    vector<int> empty;
                    return empty;
                }
            }
            delete_point.push_back(p);
        }
        return delete_point;
    }
}

/*頂点u, vがpermanentかforbiddenかを調べる.
返り値が0ならグラフのreduction不可, 負ならforbidden,　
正ならmergeでこのとき目的関数値からひくコストicpを出力.
*/
int check_unaffordable(Graph& G, int u, int v, int& obj) {
    int N = G.num_nodes;
    int icf;
    int icp;
    int sum_icf;
    int sum_icp;

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
}

/*
頂点のdelete, mergeによる番号の影響を考慮しない場合のグラフ削減アルゴリズム
*/
void reduction(Graph& G, int& obj) {
  int N = G.num_nodes;

  for(int u=0; u < N; u++) {
      vector<int> clique = delete_clique(G, u);
      if(clique.size() > 0) {
          for(int i=0; i < clique.size(); i++) {
              G.delete_node(clique[i]);
          }
      }
      for(int v=u+1; v < N; v++) {
          int cost = check_unaffordable(G, u, v, obj);
          if(cost > 0) {
              G.merge_nodes(u, v);
              obj -= cost;
          }
      }
  }
  return;
}