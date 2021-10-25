import matplotlib.pyplot as plt
import networkx as nx

G = nx.DiGraph()
G = nx.read_weighted_edgelist('lp_exact003.txt', nodetype=int)

pos = nx.spring_layout(G, k=0.7)


#weight 0.5以下のedgeを消す
removeEdges = []

for (u,v,d) in G.edges(data=True):
    if d['weight'] > 0.5 and d['weight'] <= 1:
        removeEdges.append([u,v])

#グラフから削除
G.remove_edges_from(removeEdges)

edge_labels = {(i, j): w['weight'] for i, j, w in G.edges(data=True)}
nx.draw_networkx_edge_labels(G, pos, edge_labels=edge_labels)
nx.draw_networkx(G, pos, with_labels=True, alpha=0.5)

plt.axis("off")
plt.show()