class Graph{
 public:
  int num_nodes;
  std::vector <std::vector <int>> weight; // use weight[i][j] only when i < j
  std::vector <std::vector <int>> flag; // 1 -> permitted, 0 -> forbidden, use flag[i][j] only when i < j

  Graph();  // constructor
  Graph(int n);  // constructor with n nodes
  Graph(const char *filename); // constructor from the input file
  void delete_node(int v); //; delete node v
  void merge_nodes(int a, int b); // merge nodes a and b

  void show();
};
