#include <vector>
#include <fstream>
#include <iostream>
#include <sstream>

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

void Graph::delete_node(int i){
  this->weight.erase(this->weight.begin()+i);
  this->flag.erase(this->flag.begin()+i);
  this->num_nodes--;
  
  REP(j, this->num_nodes){
    this->weight[j].erase(this->weight[j].begin()+i);
    this->flag[j].erase(this->flag[j].begin()+i);
  }

  return;
}

void Graph::merge_nodes(int a, int b){
  if(b < a) SWAP(int, a,b);
  
  REP(k, this->num_nodes){
    if(k == a || k == b) continue;

    this->weight[k][a] += this->weight[k][b];
    this->weight[a][k] = this->weight[k][a];

    if(this->flag[b][k] == -1) this->forbid(a,k);
  }

  this->delete_node(b);
  
  return;
}

void Graph::forbid(int a, int b){
  this->flag[a][b] = -1;
  this->flag[b][a] = -1;
  return;
}

void Graph::permanent(int a, int b){
  this->flag[a][b] = 1;
  this->flag[b][a] = 1;
  return;
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
