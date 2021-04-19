#include <vector>
#include <list>
#include <iostream>
#include <algorithm>
#include <ilcplex/ilocplex.h>
#include "random.hpp"

#include "main.h"
#include "graph.h"
#include "solver.h"


using namespace std;

static int rec_cnt = 0;
int random_pivot(Graph& G, const Graph& G_orig, std::vector <edge>& sol);

void ret_cnt() {
  cout << rec_cnt << endl;
}

double merge_cost(Graph& G, int u, int v) {
  double cost = 0;
  FOR(w, 0, G.num_nodes) {
    if(G.Weight(u,w) < 0 && G.Weight(v,w) < 0) cost += abs(G.Weight(u,w)) + abs(G.Weight(v,w));

    if(G.Weight(u,w) > 0 && G.Weight(v,w) > 0) cost += G.Weight(u,w) + G.Weight(v,w);

    if(G.Weight(u,w) > 0 && G.Weight(v,w) < 0 || G.Weight(u,w) < 0 && G.Weight(v,w)) {
      if(abs(G.Weight(u,w)) != G.Weight(v,w)) {
        cost += min(G.Weight(u,w), G.Weight(v,w));
      } else {
        cost += abs(G.Weight(u,w)) - 0.5;
      }
    }

  return cost;
  }
}

double f(double x, double t1, double t2) {
    if(t1 < t2) swap(t1,t2);
    return pow(x, t1) - pow(x, t1-t2) - 1;
}

double g(double x, double t1, double t2) {
    if(t1 < t2) swap(t1,t2);
    return t1 * pow(x, t1-1) - (t1-t2) * pow(x, t1-t2-1);
}

/*
 * 非線形方程式を解く（ニュートン法）
 */
double branching_number(double t1, double t2) {
  // x 初期値設定
    int k;
    double x, dx;
    x = 2.0;
    static const double eps = 1e-8; //打ち切り精度
    static const int limit = 50; //打ち切り回数
  // 打ち切り回数 or 打ち切り誤差になるまで LOOP
    for (k = 1; k <= limit; k++) {
        dx = x;
        x -= (f(x,t1,t2) / g(x,t1,t2));
        if (fabs(x - dx) / fabs(dx) < eps) {
            break;
        }
    }

  // 収束しなかった場合
    if (k > limit) {
        return -1;
    }else {
        return x;
    }

}

/*
int edge_branching(Graph& G, const Graph& G_orig, int max_obj, vector <edge>& sol) {
  rec_cnt++;

  if(G.num_nodes <= 1) return 0;

  if(G.num_nodes == 2) {
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

  vector <int> triple(3,-1);

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

  if(max_obj <= 0)  return -1;

  int best = -1;
  vector <edge> best_sol;

  double min_alpha = 5.0;
  int branch_u, branch_v;

  FOR(u, 0, G.num_nodes-1) FOR(v, u+1, G.num_nodes) {
    if(G.Weight(u,v) <= 0 || G.Flag(u,v) != 0) continue;
    double a = double(G.Weight(u,v));
    double b = merge_cost(G, u, v);
    double alpha = branching_number(a, b);
    if(alpha < min_alpha) {
      branch_u = u;
      branch_v = v;
      min_alpha = alpha;
    }
  }

  int u = branch_u;
  int v = branch_v;
  int u_name = G.node_names[u];
  int v_name = G.node_names[v];
  if(u_name > v_name) SWAP(int, u_name, v_name);

  int w = G.Weight(u,v);
  G.forbid(u, v, best_sol, G_orig);
  G.flip_edge(u,v);

  best = naive_branching(G, G_orig, max_obj-w, best_sol);
  if(best != -1) {
    best += w;
    if(best < max_obj) max_obj = best;
  }
  G.reset_flag(G.name_to_ind[u_name],G.name_to_ind[v_name]);
  G.flip_edge(G.name_to_ind[u_name],G.name_to_ind[v_name]);

  bool merge_flag = true;
  FOR(z, 0, G.num_nodes){
    if(u == z || v == z) continue;
    if((G.Flag(u,z) == -1 && G.Flag(v,z) == 1) || (G.Flag(u,z) == 1 && G.Flag(v,z) == -1)) merge_flag = false;
  }

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
*/

int naive_branching(Graph& G, const Graph& G_orig,int max_obj, vector <edge>& sol, int rec_depth){
  rec_cnt++;

  if(G.num_nodes <= 1)  return 0;

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

  if(max_obj <= 0)  return -1;

  int best = -1;
  vector <edge> best_sol;

/*
  //combine heuristic
  if(rec_depth % 3 == 0) {
    int obj = random_pivot(G, G_orig, sol);
    if(obj < max_obj) {
      max_obj = obj;
      cout << "ok" << endl;
    }
  }
*/

// edge branching

//strategy2
  vector<vector<int>> cnt_uv;

  FOR(u, 0, G.num_nodes-1) FOR(v, u+1, G.num_nodes){
    if(G.Weight(u,v) <= 0 || G.Flag(u,v) != 0) continue;
    vector<int> tmp;
    int cnt_weight = 0;
    FOR(z, 0, G.num_nodes){
      if(u == z || v == z) continue;
      if((G.Weight(u,z) > 0 && G.Weight(v,z) <= 0) || (G.Weight(u,z) <= 0 && G.Weight(v,z) > 0)) cnt_weight += abs(G.Weight(u,z)) + abs(G.Weight(v,z));
    }
    tmp.push_back(cnt_weight);
    tmp.push_back(u);
    tmp.push_back(v);
    cnt_uv.push_back(tmp);
    tmp.clear();
  }

  sort(cnt_uv.begin(), cnt_uv.end(), [](const vector<int> &alpha, const vector<int > &beta){return alpha[0] > beta[0];});

  FOR(i, 0, cnt_uv.size()){
    if(cnt_uv[i][0] > 0) {
      int u = cnt_uv[i][1];
      int v = cnt_uv[i][2];
      int u_name = G.node_names[u];
      int v_name = G.node_names[v];
      if(u_name > v_name) SWAP(int, u_name, v_name);

      int w = G.Weight(u,v);
      G.forbid(u, v, best_sol, G_orig);
      G.flip_edge(u,v);

      best = naive_branching(G, G_orig, max_obj-w, best_sol, rec_depth+1);
      if(best != -1) {
        best += w;
        if(best < max_obj) max_obj = best;
      }
      G.reset_flag(G.name_to_ind[u_name],G.name_to_ind[v_name]);
      G.flip_edge(G.name_to_ind[u_name],G.name_to_ind[v_name]);

      bool merge_flag = true;
      FOR(z, 0, G.num_nodes){
        if(u == z || v == z) continue;
        if((G.Flag(u,z) == -1 && G.Flag(v,z) == 1) || (G.Flag(u,z) == 1 && G.Flag(v,z) == -1)) merge_flag = false;
      }
      if(merge_flag){
        vector <edge> tmpsol;
        G.permanent(G.name_to_ind[u_name], G.name_to_ind[v_name], tmpsol, G_orig);
        MergeData mg(G_orig.num_nodes);
        int mergecost = G.merge_nodes(G.name_to_ind[u_name], G.name_to_ind[v_name], tmpsol, mg, G_orig);
        int cost = naive_branching(G, G_orig, max_obj-mergecost, tmpsol, rec_depth+1);
        if((cost != -1) && (best == -1 || cost + mergecost < best)){
          best = cost + mergecost;
          best_sol = tmpsol;
        }

        G.expand_nodes(u_name, v_name, mg);
        G.reset_flag(G.name_to_ind[u_name],G.num_nodes-1);
      }

      if(best != -1) sol.insert(sol.end(), best_sol.begin(), best_sol.end());
      return best;
    }else{
      break;
    }
  }


//strategy1
/*
  vector<vector<int>> cnt_uv;
  FOR(u, 0, G.num_nodes-1) FOR(v, u+1, G.num_nodes){
    if(G.Weight(u,v) <= 0 || G.Flag(u,v) != 0) continue;
    vector<int> tmp;
    int cnt = 0;
    FOR(z, 0, G.num_nodes){
      if(u == z || v == z) continue;
      if((G.Weight(u,z) > 0 && G.Weight(v,z) <= 0) || (G.Weight(u,z) <= 0 && G.Weight(v,z) > 0)) cnt++;
    }
    tmp.push_back(cnt);
    tmp.push_back(u);
    tmp.push_back(v);
    cnt_uv.push_back(tmp);
    tmp.clear();
  }



  sort(cnt_uv.begin(), cnt_uv.end(), [](const vector<int> &alpha, const vector<int > &beta){return alpha[0] > beta[0];});

  FOR(i, 0, cnt_uv.size()){
    if(cnt_uv[i][0] >= 1) {
      int u = cnt_uv[i][1];
      int v = cnt_uv[i][2];
      int u_name = G.node_names[u];
      int v_name = G.node_names[v];
      if(u_name > v_name) SWAP(int, u_name, v_name);

      int w = G.Weight(u,v);
      G.forbid(u, v, best_sol, G_orig);
      G.flip_edge(u,v);

      best = naive_branching(G, G_orig, max_obj-w, best_sol);
      if(best != -1) {
        best += w;
        if(best < max_obj) max_obj = best;
      }
      G.reset_flag(G.name_to_ind[u_name],G.name_to_ind[v_name]);
      G.flip_edge(G.name_to_ind[u_name],G.name_to_ind[v_name]);

      bool merge_flag = true;
      FOR(z, 0, G.num_nodes){
        if(u == z || v == z) continue;
        if((G.Flag(u,z) == -1 && G.Flag(v,z) == 1) || (G.Flag(u,z) == 1 && G.Flag(v,z) == -1)) merge_flag = false;
      }
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
    }else{
      break;
    }
  }

*/

}

int random_pivot(Graph& G, const Graph& G_orig, vector <edge>& sol){
  rnd.seed(RANDOM_SEED);
  int best_cost = -1;
  vector <edge> best_sol;

  REP(t, NUM_PIVOT){
    int n = G.num_nodes;
    vector <int> nodes;
    REP(i, n) nodes.push_back(i);
    vector <edge> tmpsol;
    int cost = 0;

    while(n > 1){
      int k = rnd(0, n-1);
      int pivot = nodes[k];
      vector <int> neighbor;
      nodes.erase(nodes.begin()+k);
      n--;

      for(int i=n-1; i >= 0; i--){
        if(G.Weight(nodes[i],pivot) > 0) {
          neighbor.push_back(nodes[i]);
          if(G.Flag(nodes[i],pivot) == 0){
            G.permanent(nodes[i],pivot,sol,G_orig);
            G.reset_flag(nodes[i], pivot);
          }
          nodes.erase(nodes.begin()+i);
        }
      }
      n -= neighbor.size();

      if(neighbor.size() > 1){
        FOR(i,0, (int) neighbor.size()-1) FOR(j, i+1, (int) neighbor.size()){
          if(G.Flag(neighbor[i],neighbor[j]) == 0){
            G.permanent(neighbor[i],neighbor[j],sol,G_orig);
            G.reset_flag(neighbor[i], neighbor[j]);
          }

          if(G.Weight(neighbor[i],neighbor[j]) < 0){
            cost-= G.Weight(neighbor[i],neighbor[j]);
          }
        }
      }
      if(neighbor.size() > 0 && n > 0){
        REP(i, (int) neighbor.size()) REP(j, n){
          if(G.Flag(neighbor[i],nodes[j]) == 0){
            G.forbid(neighbor[i],nodes[j],sol,G_orig);
            G.reset_flag(neighbor[i],nodes[j]);
          }

          if(G.Weight(neighbor[i],nodes[j]) > 0){
            cost += G.Weight(neighbor[i],nodes[j]);
          }
        }
      }
    }
    if(best_cost == -1 || cost < best_cost){
      best_cost = cost;
      best_sol = tmpsol;
    }
  }

  if(best_cost != -1) sol.insert(sol.end(), best_sol.begin(), best_sol.end());
  return best_cost;
}

double lp_solve(const Graph &G, vector <vector <double> >& lp_sol){
  int n = G.num_nodes;
  lp_sol = vector <vector <double> > (n, vector <double>(n, 0));

  /***/
  IloEnv env;	//enviromental object
  double obj_val = 0;

  env = IloEnv();
  IloModel model = IloModel(env);
  IloNumVarArray x (env);
  IloExprArray coef (env);
  IloExpr f (env);
  IloRangeArray constraint (env);
  //if(DEBUG) env.setOut(cerr);
  //else env.setOut(env.getNullStream());
  env.setOut(env.getNullStream());
  
  FOR (i, 0, n*(n-1)/2) x.add(IloNumVar(env , 0 , 1 , ILOFLOAT));
  
  FOR(i, 0, n-1) FOR(j, i+1, n){
    // ij corresponds to index i(2n-i-1)/2 + j - i - 1
    x.add(IloNumVar(env , 0 , 1 , ILOFLOAT) );
    if(G.Weight(i,j) > 0) f += x[i*(2*n-i-1)/2+j-i-1];
    else{
      f -= x[i*(2*n-i-1)/2+j-i-1];
      obj_val += 1;
    }
  }

  IloObjective obj(env, f, IloObjective::Minimize);
  model.add(obj);

  FOR(i, 0, n-1) FOR(j, i+1, n) FOR(k, 0, n){
    if(k == i || k == j) continue;
    if(i < k && j < k) constraint.add(x[i*(2*n-i-1)/2+j-i-1] - x[i*(2*n-i-1)/2+k-i-1] - x[j*(2*n-j-1)/2+k-j-1] <= 0);
    else if(i < k && j > k) constraint.add(x[i*(2*n-i-1)/2+j-i-1] - x[i*(2*n-i-1)/2+k-i-1] - x[k*(2*n-k-1)/2+j-k-1] <= 0);
    else if(i > k && j > k) constraint.add(x[i*(2*n-i-1)/2+j-i-1] - x[k*(2*n-k-1)/2+i-k-1] - x[k*(2*n-k-1)/2+j-k-1] <= 0);
  }
  model.add(constraint);

  IloCplex cplex = IloCplex(model);
  cplex.solve();

  obj_val += cplex.getObjValue();

  FOR(i, 0, n-1) FOR(j, i+1, n){
    lp_sol[i][j] = cplex.getValue(x[i*(2*n-i-1)/2+j-i-1]);
    lp_sol[j][i] = cplex.getValue(x[i*(2*n-i-1)/2+j-i-1]);
    // cout << i << "," << j << ": " << lp_sol[i][j] << endl;
  } 
  
  return obj_val;
}

int lp_pivot(Graph &G,  const Graph& G_orig, vector <edge>& sol, const vector <vector <double> >& lp_sol){
  rnd.seed(RANDOM_SEED);
  int best_cost = -1;
  vector <edge> best_sol;

  REP(t, NUM_PIVOT){
    int n = G.num_nodes;
    vector <int> nodes;
    REP(i, n) nodes.push_back(i);
    vector <edge> tmpsol;
    int cost = 0;

    while(n > 1){
      int k = rnd(0, n-1);
      int pivot = nodes[k];
      vector <int> neighbor;
      nodes.erase(nodes.begin()+k);
      n--;
      neighbor.push_back(pivot);

      for(int i=n-1; i >= 0; i--){
        if(rnd(0,0xffffff) > lp_sol[nodes[i]][pivot] * 0xffffff) {
          neighbor.push_back(nodes[i]);
          nodes.erase(nodes.begin()+i);
          n--;
        }
      }

      if(neighbor.size() > 1){
        FOR(i,0, (int) neighbor.size()-1) FOR(j, i+1, (int) neighbor.size()){
          if(G.Flag(neighbor[i],neighbor[j]) == 0){
            G.permanent(neighbor[i],neighbor[j],sol,G_orig);
            G.reset_flag(neighbor[i], neighbor[j]);
          }
          if(G.Weight(neighbor[i],neighbor[j]) < 0){
            cost-= G.Weight(neighbor[i],neighbor[j]);
          }
        }
      }
      if(neighbor.size() > 0 && n > 0){
        REP(i, (int) neighbor.size()) REP(j, n){
          if(G.Flag(neighbor[i],nodes[j]) == 0){
            G.forbid(neighbor[i],nodes[j],sol,G_orig);
            G.reset_flag(neighbor[i],nodes[j]);
          }
          if(G.Weight(neighbor[i],nodes[j]) > 0){
            cost += G.Weight(neighbor[i],nodes[j]);
          }
        }
      }
    }
    if(best_cost == -1 || cost < best_cost){
      best_cost = cost;
      best_sol = tmpsol;
    }
  }
      

  if(best_cost != -1) sol.insert(sol.end(), best_sol.begin(), best_sol.end());
  return best_cost;
}