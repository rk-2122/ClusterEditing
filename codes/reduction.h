std::vector <int> search_clique(const Graph& G, const int u);

int check_unaffordable(Graph&G, const int u, const int v, const int obj);

int reduction(Graph& G, const Graph& G_orig, const int obj, std::vector <edge>& sol);

int cal_reduction(Graph& G, const Graph& G_orig, const int obj, std::vector <edge>& sol);
