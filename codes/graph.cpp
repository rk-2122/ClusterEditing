#include <vector>
#include <fstream>
#include <iostream>
#include <string>
#include <sstream>

#include "main.h"
#include "graph.h"

using namespace std;

Graph::Graph(){
  this-> num_nodes = 0;
}

Graph::Graph(int n){
  this->num_nodes = n;
  this->weight = vector <vector <int>>(n, vector <int>(n,0));
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
	  this->weight = vector <vector <int>>(N, vector <int>(N,0));
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
      int aa = MIN(a,b);
      int bb = MAX(a,b);
      this -> weight[aa-1][bb-1] = 1;
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
  int i = MIN(a,b);
  int j = MAX(a,b);

  REP(k, i){
    this->weight[k][i] += this->weight[k][j];
  }

  FOR(k, i, j){
    this->weight[i][k] += this->weight[k][j];
  }

  FOR(k, j, this->num_nodes){
    this->weight[i][k] += this->weight[j][k];
  }

  // to do: modify flags
  
  this->delete_node(j);
  
  return;
}

void Graph::show(){
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
