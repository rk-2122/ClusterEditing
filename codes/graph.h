class Graph{
 public:
  int num_nodes;
  std::vector <std::vector <int>> weight; 
  std::vector <std::vector <int>> flag; // 1 -> permanent, -1 -> forbidden, 0 -> not restricted

  Graph();  // constructor
  Graph(int n);  // constructor with n nodes
  Graph(const char *filename); // constructor from the input file
  void delete_node(int v); //; delete node v
  void merge_nodes(int a, int b); // merge nodes a and b
  void forbid(int a, int b);
  void permanent(int a, int b);
  
  // return true if a conflict triple exists, false otherwise
  bool conflict_triple(std::vector <int>& triple) const;
  
  
  // print weights
  void show() const;
};
