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
import threading
import os

class Req():
    def __init__(self, time, type, weight, bench):
        self.bench = bench
        self.time = time
        self.type = type
        self.weight = weight

firebase_admin.initialize_app(options={
    'databaseURL': 'https://dumbot-3c243.firebaseio.com/'
})

DEMO = db.reference('demo2')

request = Req(time=int(time.time()), type='delivering', weight='1',bench = 'B7')
DEMO.child('requests').child(str(int(time.time()*1000))).set(request.__dict__)

time.sleep(15)

request = Req(time=int(time.time()), type='delivering', weight='1.5', bench = 'B9')
DEMO.child('requests').child(str(int(time.time()*1000))).set(request.__dict__)

time.sleep(10)

request = Req(time=int(time.time()), type='collecting', weight='0.5', bench = 'B12')
DEMO.child('requests').child(str(int(time.time()*1000))).set(request.__dict__)

time.sleep(10)

request = Req(time=int(time.time()), type='delivering', weight='1.5', bench = 'B10')
DEMO.child('requests').child(str(int(time.time()*1000))).set(request.__dict__)

time.sleep(20)

request = Req(time=int(time.time()), type='delivering', weight='1.5', bench = 'B13')
DEMO.child('requests').child(str(int(time.time()*1000))).set(request.__dict__)

time.sleep(20)

request = Req(time=int(time.time()), type='collecting', weight='1', bench = 'B7')
DEMO.child('requests').child(str(int(time.time()*1000))).set(request.__dict__)

time.sleep(5)

request = Req(time=int(time.time()), type='collecting', weight='1.5', bench = 'B9')
DEMO.child('requests').child(str(int(time.time()*1000))).set(request.__dict__)

time.sleep(10)

request = Req(time=int(time.time()), type='delivering', weight='1.5', bench = 'B12')
DEMO.child('requests').child(str(int(time.time()*1000))).set(request.__dict__)

time.sleep(10)

request = Req(time=int(time.time()), type='delivering', weight='1.5', bench = 'B10')
DEMO.child('requests').child(str(int(time.time()*1000))).set(request.__dict__)
