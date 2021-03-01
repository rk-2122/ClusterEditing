void reduction(Graph& G, int& obj);


// return:
//  - the opt val if it is <= max_obj
//  - -1 if opt val > max_obj
// assumption: max_obj >= 0
int naive_branching(Graph& G, const Graph& G_orig, int max_obj, std::vector <edge>& sol);
