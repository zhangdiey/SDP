import flask
from flask_jsonpify import jsonify
import firebase_admin
from firebase_admin import db
import requests

static_dir = "static"

app = flask.Flask(__name__, static_folder=static_dir)

firebase_admin.initialize_app(options={
    'databaseURL': 'https://dumbot-3c243.firebaseio.com/'
})

DEMO = db.reference('demo1')

# demo1 has two childs:
# cur_cmd (g or h)
# alert (True or False)

@app.route('/')
def test():
    cur_cmd = getCurrentCMD()
    updateAlert(False)
    sendCMD(cur_cmd)

    return cur_cmd

def updateAlert(status):
    DEMO.update({'alert':f'{status}'})

def getCurrentCMD():
    return DEMO.child('cur_cmd').get()

def sendCMD(cmd):
    dictToSend = {'cmd':cmd}
    res = requests.post('http://0.0.0.0:3142/', json=dictToSend)
    print ('response from server:', res.text)


if __name__ == '__main__':
    app.run(debug=True, host='0.0.0.0', port=1337)
