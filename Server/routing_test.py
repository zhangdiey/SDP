from ortools.graph import pywrapgraph
def cost_callback(from_node,to_node):
    return from_node==0 & to_node==1
ham = pywrapgraph.HamiltonianPathSolver(2,cost_callback)
