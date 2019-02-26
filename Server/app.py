import flask
from flask_jsonpify import jsonify
import firebase_admin
from firebase_admin import db
import requests
import networkx as nx
import json
import ast
import time
from request import Request
import routing

static_dir = "static"

app = flask.Flask(__name__, static_folder=static_dir)

firebase_admin.initialize_app(options={
    'databaseURL': 'https://dumbot-3c243.firebaseio.com/'
})

# DEMO = db.reference('demo1') # for demo1
DEMO = db.reference('demo2')

# store the requests
requests = []
requests_id = []
buffered_requests = []
buffered_id = []
cur_request = None
num_total_valid_request = 0

# store the layout
layoutGraph = nx.Graph()
shortest_paths = None

# store the command
command = []

# store the last edge
last_edge = ('SA2','5')

# hyper-parameters
alpha = 0.003

# demo2 has five childs:
# cur_request (json)
# alert (True or False)
# layout: the gym layout (json)
# log: logfile for request history (json)
# requests: all valid requests (json)

# for demo1
# @app.route('/sendCMD')
# def send_main():
#     cur_cmd = getCurrentCMD()
#     updateAlert(False)
#     sendCMD(cur_cmd)
#     DEMO.child('cur_cmd').listen(listener)
#
#     return cur_cmd

@app.route('/')
def initialize_main():
    # set two listeners
    try:
        DEMO.child('layout').listen(layoutListener)
        DEMO.child('requests').listen(requestsListener)
    except Exception as e:
        print(e)
        return "Failed to initialize. Error {e}."
    return "Server initialized."

@app.route('/getAlert',methods=['POST'])
def get_main():
    status = flask.request.get_json(force=True)['alert']
    print ('data from client:', status)
    if (isValidSTATUS(status)):
        updateAlert(status)
        return "VALID STATUS"
    else:
        return "INVALID STATUS"

def updateAlert(status):
    DEMO.update({'alert':f'{status}'})

# for demo1
# def getCurrentCMD():
#     return DEMO.child('cur_cmd').get()

def isValidSTATUS(status):
    return status in ['True','False']

def sendCMD(cmd):
    dictToSend = {'cmd':cmd}
    # res = requests.post('http://patamon.inf.ed.ac.uk:3142/getCMD', json=dictToSend)
    # for testing
    res = requests.post('http://0.0.0.0:3142/getCMD', json=dictToSend)
    print ('response from server:', res.text)

# for demo1
# def listener(event):
#     # print(event.event_type)  # can be 'put' or 'patch'
#     # print(event.path)  # relative to the reference, it seems
#     # print(event.data)  # new data at /reference/event.path. None if deleted
#     cur_cmd = getCurrentCMD()
#     sendCMD(cur_cmd)

def layoutListener(event):
    # print(event.event_type)  # can be 'put' or 'patch'
    # print(event.path)  # relative to the reference, it seems
    # print(event.data)  # new data at /reference/event.path. None if deleted
    layout = DEMO.child('layout').get()
    global layoutGraph
    global shortest_paths
    nodes = ast.literal_eval(layout["nodes"])
    edges = ast.literal_eval(layout["edges"])
    angles = ast.literal_eval(layout["angles"])
    layoutGraph.add_nodes_from(nodes)
    layoutGraph.add_edges_from(edges)
    # print("Nodes:", G.nodes)
    # print("Edges:", G.edges)
    for edge in layoutGraph.edges:
        layoutGraph.edges[edge]['angle'] = angles[edge]
    shortest_paths = dict(nx.all_pairs_dijkstra(layoutGraph))

def requestsListener(event):
    # print(event.event_type)  # can be 'put' or 'patch'
    # print(event.path)  # relative to the reference, it seems
    # print(event.data)  # new data at /reference/event.path. None if deleted
    global requests
    global command
    global buffered_requests
    global requests_id
    global cur_request
    global num_total_valid_request
    global last_edge
    global shortest_paths
    global alpha

    reqs = DEMO.child('requests').get()
    if (reqs == None):
        print('Waiting for the first request.')
        return
    if (len(reqs) != num_total_valid_request):
        num_total_valid_request = len(reqs)
        if requests == []:
            # req is the ID
            for req in reqs:
                requests_id.append(req)
                request = makeRequestObject(reqs,req)
                requests.append(request)
            # Dewi's Code
            requests.sort(key=(lambda x:x.time))
            schedule = routing.solve_schedule(last_edge[1],
                                                set(requests),
                                                routing.make_request_eval(shortest_paths,
                                                                                alpha,
                                                                                requests[len(requests)-1].time))
            route = routing.schedule_to_route_schedule(last_edge[1],schedule,shortest_paths)
            command = routing.route_schedule_to_pi_schedule(route,last_edge,layoutGraph)
            print("Cost original: "),
            print(routing.schedule_cost(last_edge[1],requests,routing.make_request_eval(shortest_paths,alpha,requests[len(requests)-1].time)))
            print("Cost alternative: "),
            print(routing.schedule_cost(last_edge[1],schedule,routing.make_request_eval(shortest_paths,alpha,requests[len(requests)-1].time)))
            requests=schedule
            cur_request = requests[0]
            changeCurRequest(cur_request)
            print('len of requests',len(requests))
        else:
            for req in reqs:
                if req in requests_id:
                    print(req,'is already processed.')
                else:
                    if req in buffered_id:
                        print(req,'is already in buffer.')
                    else:
                        request = makeRequestObject(reqs,req)
                        buffered_requests.append(request)
                        buffered_id.append(req)
                        print(req,'goes to buffer.')
            print('len of buffer',len(buffered_requests))
    else:
        print('No changes in requests.')

def makeRequestObject(reqs,req):
    type = reqs[req]['type']
    bench = reqs[req]['bench']
    time = reqs[req]['time']
    weight = reqs[req]['weight']
    id = req
    if weight == '0.5':
        SA = 'SA1'
    elif weight == '1':
        SA = 'SA2'
    else:
        SA = 'SA3'

    if type == 'delivering':
        return Request(type=type,time=time,id=id,s_point=SA,e_point=bench,weight=weight)
    else:
        return Request(type=type,time=time,id=id,s_point=bench,e_point=SA,weight=weight)

def deleteRequest(request):
    DEMO.child('requests').child(request).delete()

def changeCurRequest(request):
    DEMO.child('cur_request').update(request.__dict__)
    DEMO.child('cur_request').update({'status':'True'})

def removeCurRequest():
    DEMO.child('cur_request').update({'status':'False'})

def addToLog():
    global cur_request
    global requests
    requests.remove(cur_request)
    print(len(requests))
    DEMO.child('log').child(cur_request.id).set(cur_request.__dict__)
    if len(requests)>0:
        cur_request = requests[0]
        changeCurRequest(cur_request)
    else:
        removeCurRequest()

if __name__ == '__main__':
    # app.run(debug=True, host='rudedski.inf.ed.ac.uk', port=1337)
    # for testing
    app.run(debug=True, host='0.0.0.0', port=1337)
