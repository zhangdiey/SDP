import networkx as nx
import math
import functools
import copy
def make_request_eval(shortest_paths,alpha,time_zero):
    def request_eval(prev_request_eval,request):
        (previous_node,schedule_time,schedule_cost,wait_times)=prev_request_eval
        travel_time=shortest_paths[previous_node][0][request.start_node]
        execution_time=shortest_paths[request.start_node][0][request.end_node]
        schedule_time+=travel_time+execution_time
        waiting_time=schedule_time+time_zero-request.time
        schedule_cost+=math.exp(alpha*waiting_time)*(travel_time+execution_time)
        wait_times.append((request.id,waiting_time))
        return (request.end_node,schedule_time,schedule_cost,wait_times)
    return request_eval
def solve_schedule(start_node,requests,request_eval):
    best_schedule=sorted(requests,key=(lambda x:x.time))
    best_schedule_cost=functools.reduce(request_eval,best_schedule,(start_node,0,0,[]))[2]
    queue=[([],requests,(start_node,0,0,[]))]
    while queue != []:
        (schedule,remaining_requests,(last_node,schedule_time,schedule_cost,wait_times))=queue.pop()
        if schedule_cost<best_schedule_cost:
            if remaining_requests==set([]):
                best_schedule=schedule
                best_schedule_cost=schedule_cost
            else:
                for request in remaining_requests:
                    queue.append((schedule+[request],
                                  remaining_requests-set([request]),
                                  request_eval((last_node,schedule_time,schedule_cost,wait_times),request)))
    return best_schedule
def schedule_to_route_schedule(start_node,schedule,shortest_paths):
    start=start_node
    route_schedule=[]
    for request in schedule:
        route_schedule.append((shortest_paths[start][1][request.start_node],shortest_paths[request.start_node][1][request.end_node],request.id))
        start=request.end_node
    return route_schedule
def schedule_cost(start_node,schedule,request_eval):
    return functools.reduce(request_eval,schedule,(start_node,0,0,[]))
def route_schedule_to_pi_schedule(route_schedule,prev_edge,graph):
    pi_schedule=[]
    for (transition,execution,id) in route_schedule:
        transition_route=[]
        transition_copy=copy.deepcopy(transition)
        transition_copy.pop(0)
        for (start,end) in zip(transition,transition_copy):
            rotation=graph[prev_edge[0]][prev_edge[1]]['angle'].get((start,end))
            if rotation is None:
                rotation=graph[prev_edge[0]][prev_edge[1]]['angle'].get((end,start))
            if rotation != 'F':
                transition_route.append((rotation,start))
            transition_route.append(('F',end))
            prev_edge=(start,end)
        transition_route.append(('U',prev_edge[1])) #changed by Moy
        execution_route=[]
        execution_copy=copy.deepcopy(execution)
        execution_copy.pop(0)
        for (start,end) in zip(execution,execution_copy):
            rotation=graph[prev_edge[0]][prev_edge[1]]['angle'].get((start,end))
            if rotation is None:
                rotation=graph[prev_edge[0]][prev_edge[1]]['angle'].get((end,start))
            if rotation != 'F':
                execution_route.append((rotation,start))
            execution_route.append(('F',end))
            prev_edge=(start,end)
        execution_route.append(('D',prev_edge[1])) #changed by Moy
        pi_schedule.append((transition_route,execution_route,id))
    return pi_schedule
