from flask import Flask, request
from flask_jsonpify import jsonify
from utils import *
import requests
import serial
import time

static_dir = "static"

app = Flask(__name__, static_folder=static_dir)

# All Arduino related content are marked for further test with hardware

# ser = serial.Serial("/dev/ttyACM0",9600)  #change ACM number as found from ls /dev/tty/ACM*
# ser.baudrate=9600

@app.route('/', methods=['POST'])
def main_post():
    cmd = request.get_json(force=True)['cmd']
    print ('data from client:', cmd)
    if (isValidCMD(cmd)):
        # ser.write(bytes(cmd, 'utf-8'))
        return "VALID CMD"
    else:
        return "INVALID CMD"

if __name__ == '__main__':
    app.run(debug=True, host='0.0.0.0', port=3142)
