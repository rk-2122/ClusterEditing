
import sys
import networkx as nx 
import matplotlib.pyplot as plt

G = nx.read_edgelist(sys.argv[2], comments="p", nodetype=int)
for (a,b) in G.edges:
    G[a][b]['color'] = 'black'

if len(sys.argv) >= 4:
    f = open(sys.argv[3], 'r')
    d = f.readlines()
    for j in d:
        aa, bb = j.split()
        a = int(aa)
        b = int(bb)

        if (a,b) in G.edges:
            G[a][b]['color'] = 'b'
        else:
            G.add_edge(a,b, color="r")

colors = [G[u][v]['color'] for u,v in G.edges]
nx.draw_networkx(G,edge_color=colors)
plt.savefig(sys.argv[1]) 
