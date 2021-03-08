class Graph{
 public:
  int num_nodes;
  std::vector <std::vector <int>> weight;
  std::vector <std::vector <int>> flag; // 1 -> permanent, -1 -> forbidden, 0 -> not restricted
  std::vector <int> node_names;
  std::vector <int> node_pointers;

  Graph();  // constructor
  Graph(int n);  // constructor with n nodes
  Graph(const char *filename); // constructor from the input file

  void delete_node(int i, std::vector <edge>& sol, const Graph &G_orig);  // i is the current index
  void delete_nodes(std::vector <int> U, std::vector <edge>& sol, const Graph &G_orig);

  // merge nodes a and b (current indices), return its cost
  int merge_nodes(int a, int b, std::vector <edge> & sol, const Graph &G_orig);

  void forbid(int a, int b, std::vector <edge>& sol, const Graph &G_orig); // a,b: current indices
  void permanent(int a, int b, std::vector <edge>& sol, const Graph &G_orig); // a,b: current indices
  void delete_edge(int a, int b); // a,b: current indices
  void add_edge(int a, int b); // a,b: current indicies
  void flip_edge(int a, int b); // a,b: current indicies


  // return false if no conflict triple exists,
  //        true and set triple to a conflict triple otherwise
  bool conflict_triple(std::vector <int>& triple) const;


  // print weights
  void show() const;
};
