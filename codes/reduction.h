vector <int> search_clique(Graph& G, int u);

int check_unaffordable(Graph&G, int u, int v, int& obj);

int reduction(Graph& G, const Graph& G_orig, int obj, vector <edge>& sol);

void cal_reduction(Graph& G, const Graph& G_orig, int obj, vector <edge>& sol);
