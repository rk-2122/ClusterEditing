# data structure of graphs

- Nodes are counted from 0 through n-1, inclusive.

- vector <vector <int>> weight stores edge weights. weight[i][j] is used only when i < j.

- vector <vector <int>> flag stores edge flags. flag[i][j] = 1 -> edge ij is permitted, flag[i][j] = 0 -> edge ij is forbidden. flag[i][j] is used only when i < j.
