#include <iostream>
#include <fstream>
#include <vector>
#include <list>
#include <string>
#include <time.h>

#include "cmdline.h"

#include "main.h"
#include "graph.h"
#include "solver.h"
#include "reduction.h"


using namespace std;

int main(int argc, char *argv[]){
  // parser
  cmdline::parser op;
  op.add<string>("input", 'i', "input file name", false);
  op.add<string>("output", 'o', "output fine name", false);
  op.add("reduction", 'r', "reduction-only mode");
  op.add("help", 0, "print help");

  if (!op.parse(argc, argv)||op.exist("help")){
    std::cout<<op.error_full()<<op.usage();
    return 0;
  }

  // input
  Graph Gin;

  if(op.exist("input")){
    const char *fn = op.get<string>("input").c_str();
    Gin = Graph(fn);
  }
  else Gin = Graph("../instances/exact/exact003.gr");

  vector <edge> sol;

  //////////////////// begin
  clock_t c_start = clock();

  Graph G = Gin;
  vector <vector <double> > lp_sol;
  vector <edge> lp_solve_sol;
  double lp_obj = lp_solve(Gin, lp_sol);
  vector <edge> sol1;
  int obj1 = lp_pivot(G, Gin, sol1, lp_sol);
  vector <edge> random_pivot_sol;
  int random_pivot_obj = random_pivot(G, Gin, random_pivot_sol);
  cout << op.get<string>("input") << lp_obj << " " << obj1 << " " << random_pivot_obj << endl;
  //cout << op.get<string>("input") << " "<< lp_obj << endl;

  /*
  for(int i=0; i<lp_sol.size(); i++) {
    for(int j=0; j<lp_sol[i].size(); j++) {
      if (lp_sol[i][j] < 10e-10) lp_sol[i][j] = 0;
      //cout << lp_sol[i][j] << " ";
      if (lp_sol[i][j] == 1 && G.weight[i][j] == 1)  {
        //cnt++;
        lp_solve_sol.push_back(edge(i, j));
      }
      if (lp_sol[i][j] == 0 && G.weight[i][j] == -1) {
        //cnt ++;
        lp_solve_sol.push_back(edge(i, j));
      }
    }
    //cout << endl;
  }
  */
  //cout << cnt << endl;
  //show_sol(lp_solve_sol);
/*
  if(obj1 < random_pivot_obj){
    obj1 = int(lp_obj+1);
    cout << obj1 << endl;
    //sol1 = lp_solve_sol;
  }
  vector <edge> sol2;
  int red_cost = cal_reduction(G, Gin, obj1, sol2);
  red_cost += cal_ker(G, Gin, sol2);
  if(op.exist("reduction")){
    clock_t c_end = clock();
    cout << op.get<string>("input") << " " << obj1 << " " << red_cost << " " << Gin.num_nodes << " " << G.num_nodes << " " << (double) (c_end - c_start)/ CLOCKS_PER_SEC << endl;
    return 0;
  }

  int rec_depth = 0;
  vector<vector<int> > uv;
  uv.clear();
  int obj2 = naive_branching(G, Gin, obj1-red_cost, sol2, rec_depth, uv);

  if (obj1 > obj2 + red_cost && obj2 != -1){
    obj1 = obj2 + red_cost;
    sol1 = sol2;
    }

  clock_t c_end = clock();
  if(DEBUG){
    double elapsed = (double) (c_end - c_start) / CLOCKS_PER_SEC;
    cout << op.get<string>("input") << " " << obj1 << " " << elapsed << endl;
    //cout << op.get<string>("input") << " "<< obj1 << " "<< (double) (c_end - c_start)/ CLOCKS_PER_SEC << endl;
    //show_sol(sol1);
    //ret_cnt();
  }

  if(op.exist("output")){
    const char *fo = op.get<string>("output").c_str();
    write_sol(sol1,fo);
  }
*/
  /*
  if(obj1 > random_pivot_obj){
    obj1 = random_pivot_obj;
    sol1 = random_pivot_sol;
  }

  vector <edge> sol2;
  int red_cost = cal_reduction(G, Gin, obj1, sol2);
  red_cost += cal_ker(G, Gin, sol2);
  //cout << red_cost << endl;
  if(op.exist("reduction")){
    clock_t c_end = clock();
    cout << op.get<string>("input") << " " << obj1 << " " << red_cost << " " << Gin.num_nodes << " " << G.num_nodes << " " << (double) (c_end - c_start)/ CLOCKS_PER_SEC << endl;
    return 0;
  }

  int rec_depth = 0;
  vector<vector<int> > uv;
  uv.clear();
  //cout << "floor lp_obj " << floor(lp_obj) << endl;
  //cout << "ceil lp_obj " << ceil(lp_obj) << endl;
  int obj2 = naive_branching(G, Gin, obj1-red_cost, sol2, rec_depth, uv);

  if (obj1 > obj2 + red_cost && obj2 != -1){
    obj1 = obj2 + red_cost;
    sol1 = sol2;
  }
  clock_t c_end = clock();
  if(DEBUG){
    double elapsed = (double) (c_end - c_start) / CLOCKS_PER_SEC;
    cout << op.get<string>("input") << " "<< obj1 << " "<< (double) (c_end - c_start)/ CLOCKS_PER_SEC << endl;
    ret_cnt();
    //cout << (ret_time() / elapsed) * 100 << endl;
  }

  if(op.exist("output")){
    const char *fo = op.get<string>("output").c_str();
    write_sol(sol1,fo);
  }
  */
}


void show_sol(const vector <edge>& sol){
  for (auto u: sol){
    cout << u.first+1 << " " << u.second+1 << endl;
  }

  return;
}

void write_sol(const vector <edge>& sol, const char *fo){
  ofstream ot(fo);

  for (auto u: sol){
    ot << u.first+1 << " " << u.second+1 << endl;
  }
  ot.close();
}
