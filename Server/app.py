import flask
from flask_jsonpify import jsonify
import firebase_admin
from firebase_admin import db
import requests
import networkx as nx
import json
import ast

static_dir = "static"

app = flask.Flask(__name__, static_folder=static_dir)

firebase_admin.initialize_app(options={
    'databaseURL': 'https://dumbot-3c243.firebaseio.com/'
})

# DEMO = db.reference('demo1') # for demo1
DEMO = db.reference('demo2')



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
    print(layout["nodes"])
    G = nx.Graph()
    nodes = ast.literal_eval(layout["nodes"])
    edges = ast.literal_eval(layout["edges"])
    G.add_nodes_from(nodes)
    G.add_edges_from(edges)
    print("Nodes:", G.nodes)
    print("Edges:", G.edges)

def requestsListener(event):
    # print(event.event_type)  # can be 'put' or 'patch'
    # print(event.path)  # relative to the reference, it seems
    # print(event.data)  # new data at /reference/event.path. None if deleted
    requests = DEMO.child('requests').get()
    print(requests)

if __name__ == '__main__':
    # app.run(debug=True, host='rudedski.inf.ed.ac.uk', port=1337)
    # for testing
    app.run(debug=True, host='0.0.0.0', port=1337)
