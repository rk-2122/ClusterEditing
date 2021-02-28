#include <vector>
#include <fstream>
#include <iostream>
#include <sstream>
#include <algorithm>

#include "main.h"
#include "graph.h"

using namespace std;

Graph::Graph(){
  this-> num_nodes = 0;
}

Graph::Graph(int n){
  this->num_nodes = n;
  this->weight = vector <vector <int>>(n, vector <int>(n,-1));
  this->flag = vector <vector <int>>(n, vector <int>(n,0));

  this->node_names = vector <int>(n);
  this->node_pointers = vector <int>(n);

  REP(i, n){
    this->node_names[i] = i;
    this->node_pointers[i] = i;
  }

}

Graph::Graph(const char *filename){

  ifstream ifs(filename);
  string line;
  int N, M;

  if(!ifs){
    cerr << "Error: file not opened." << endl;
    return;
  }

  int num = 0;

  while(getline(ifs, line)){
    if(line[0] == 'c') continue;

    if(line[0] == 'p'){
      stringstream s{line};
      int cnt = 0;
      while(getline(s, line, ' ')){
	      if(cnt == 2){
	        N = stoi(line);
	        this->num_nodes = N;
	        this->weight = vector <vector <int>>(N, vector <int>(N,-1));
	        this->flag = vector <vector <int>>(N, vector <int>(N,0));
          this->node_names = vector <int>(N);
          this->node_pointers = vector <int>(N);

          REP(i, N){
            this->node_names[i] = i;
            this->node_pointers[i] = i;
          }
	      }
	      else if(cnt == 3){
	        M = stoi(line);
	      }
	      cnt++;
      }
    }
    else{
      stringstream ss;
      int a, b;
      ss << line;
      ss >> a;
      ss.ignore();
      ss >> b;
      this -> weight[a-1][b-1] = 1;
      this -> weight[b-1][a-1] = 1;
    }

    num++;
  }

  ifs.close();
}

void Graph::delete_node(int i, vector <edge>& sol, const Graph &G_orig){
  for(int &u : this->node_pointers) if(u == this->node_names[i]) u = -1;
  REP(j, this->num_nodes){
    if(j == i) continue;
    if(this->flag[i][j] == 0) this->forbid(i,j,sol, G_orig);
  }
  this->weight.erase(this->weight.begin()+i);
  this->flag.erase(this->flag.begin()+i);
  this->num_nodes--;
  this->node_names.erase(this->node_names.begin()+i);

  REP(j, this->num_nodes){
    this->weight[j].erase(this->weight[j].begin()+i);
    this->flag[j].erase(this->flag[j].begin()+i);
  }

  return;
}

void Graph::delete_nodes(vector <int> U, vector <edge>& sol, const Graph &G_orig){
  sort(U.begin(), U.end());

  REP(i, U.size()){
    j = i+1;
    REP(k, this->num_nodes){
      if(k == U[i]) continue;

      if(j < U.size() && U[j]==k){
        this->permanent(U[i], k, sol, G_orig);
        j++;
      }
      else{
        this->forbid(U[i], k, sol, G_orig);
      }
    }
  }

  for(int i = U.size()-1; i >= 0; i--){
    this->weight.erase(this->weight.begin()+U[i]);
    this->flag.erase(this->flag.begin()+U[i]);
    this->num_nodes--;
    this->node_names.erase(this->node_names.begin()+U[i]);

    REP(j, this->num_nodes){
      this->weight[j].erase(this->weight[j].begin()+U[i]);
      this->flag[j].erase(this->flag[j].begin()+U[i]);
    }
  }

  return;
}


int Graph::merge_nodes(int a, int b, vector <edge> &sol, const Graph &G_orig){
  int ans = 0;
  if(b < a) SWAP(int, a,b);

  REP(k, this->num_nodes){
    if(k == a || k == b) continue;
    if(this->flag[b][k] == -1 && this->flag[a][k] != -1) this->forbid(a,k,sol,G_orig);
    if(this->flag[a][k] == -1 && this->flag[b][k] != -1) this->forbid(b,k,sol,G_orig);
    if(this->weight[k][a]*this->weight[k][b] < 0) ans += min(abs(this->weight[k][a]), abs(this->weight[k][b]));
    this->weight[k][a] += this->weight[k][b];
    this->weight[a][k] = this->weight[k][a];
  }

  for(int &u : this->node_pointers) if(u == this->node_names[b]) u = this->node_names[a];
  this->weight.erase(this->weight.begin()+b);
  this->flag.erase(this->flag.begin()+b);
  this->num_nodes--;
  this->node_names.erase(this->node_names.begin()+b);

  REP(j, this->num_nodes){
    this->weight[j].erase(this->weight[j].begin()+b);
    this->flag[j].erase(this->flag[j].begin()+b);
  }
  return ans;
}

void Graph::forbid(int a, int b, vector <edge>& sol, const Graph& G_orig){
  this->flag[a][b] = -1;
  this->flag[b][a] = -1;

  vector <int> A, B;
  REP(i, node_pointers.size()){
    if(node_pointers[i] == a) A.push_back(i);
    if(node_pointers[i] == b) B.push_back(i);
  }

  for(auto i: A) for(auto j: B){
    if (G_orig.weight[i][j] > 0) sol.push_back(make_pair(i,j));
  }
  return;
}

void Graph::permanent(int a, int b, vector <edge> & sol, const Graph &G_orig){
  this->flag[a][b] = 1;
  this->flag[b][a] = 1;

  vector <int> A, B;
  REP(i, node_pointers.size()){
    if(node_pointers[i] == a) A.push_back(i);
    if(node_pointers[i] == b) B.push_back(i);
  }

  for(auto i: A) for(auto j: B){
    if (G_orig.weight[i][j] < 0) sol.push_back(make_pair(i,j));
  }
  return;
}


void Graph::delete_edge(int a, int b){
  this->weight[a][b] = -1;
  this->weight[b][a] = -1;
  return;
}


void Graph::add_edge(int a, int b){
  this->weight[a][b] = 1;
  this->weight[b][a] = 1;
  return;
}


bool Graph::conflict_triple (vector <int>& triple) const{
  FOR(u, 0, this->num_nodes) FOR(v, 0, this->num_nodes){
    if (u == v || this->weight[u][v] <= 0 || this->flag[u][v] == -1) continue;

    FOR(w, 0, this->num_nodes){
      if(u == w || v == w) continue;

      if(this->weight[u][w] > 0 && this->flag[u][w] >= 0 && (this->weight[v][w] <= 0 || this->flag[v][w] == -1)){
	      triple.clear();
	      triple.push_back(u);
	      triple.push_back(v);
	      triple.push_back(w);
	      return true;
      }
    }
  }

  return false;
}

void Graph::show () const{
  int n = this->num_nodes;

  FOR(i, 0, n){
    cout << i << ": ";
    FOR(j, 0, n){
      cout << j << "," << this->weight[i][j] << "; ";
    }
    cout << endl;
  }

  return;
}
