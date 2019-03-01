import routing as r
import numpy as np
from request import Request
from sortedcontainers import SortedDict
import networkx as nx
def simulate(arrival_rate,exercise_duration,exercise_number,alpha,graph,start_node,duration,mean_fitness,std_fitness):
    arrival_duration = 1/arrival_rate
    geom_p = 1/(exercise_number+1)
    queue = 0
    available_benches = set([node for node in graph.nodes if node[0]=='B'])
    occupied_benches = dict([])
    available_weights = SortedDict(zip(map(lambda x:x*5,range(len(graph.nodes))),filter(lambda x:x[0]=='S',graph.nodes)))
    events=[]
    time=np.random.exponential(arrival_duration)
    event='arrival'
    requests=set([])
    schedule_items=0
    cur_node = start_node
    shortest_paths = nx.all_pairs_dijkstra(graph)
    while time < duration:
        event_type=event[0]
        if event_type == 'arrival':
            if len(available_benches) == 0:
                queue+=1
            else:
                fitness=np.random.normal(mean_fitness,std_fitness)
                bench=available_benches.pop()
                occupied_benches[bench]=fitness
                max_weight=np.random.uniform(high=2.0)*fitness
                weight=next(available_weights.irange(max_weight,reverse=true)))
                requests.add(Request(time,'delivery',weight,available_weights.pop(weight),bench))
            next_arrival=np.random.exponential(arrival_duration)
        elif event_type == 'departure':
            if queue > 0 :
                queue-=1
                event=('arrival')
                break
        elif event_type == 'delivery_request':
            bench=event[1]
            fitness=occupied_benches[bench]
            max_weight=np.random.uniform(high=2.0)*fitness
            weight=next(available_weights.irange(max_weight,reverse=true)))
            requests.add(Request(time,'delivery',weight,available_weights.pop(weight),bench))
            if schedule_items==0:
                r.solve_schedule(
        elif event_type == 'delivery_completed':
            
        (time,event)=events.pop()

        
        
