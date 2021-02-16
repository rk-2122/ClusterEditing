# data structure of graphs

- Nodes are counted from 0 through n-1, inclusive.

- vector <vector <int>> weight stores edge weights. weight[i][j] = weight[j][i].

- vector <vector <int>> flag stores edge flags. flag[i][j] = 1 -> edge ij is parmanent, flag[i][j] = 0 -> edge ij is forbidden. flag[i][j] = flag[j][i];

- When nodes a and b are merged and a < b, b is removed.
