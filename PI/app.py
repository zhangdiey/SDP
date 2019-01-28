from flask import Flask, request
from flask_jsonpify import jsonify
from utils import *
import serial
import time

static_dir = "static"

app = Flask(__name__, static_folder=static_dir)

ser = serial.Serial("/dev/ttyACM0",9600)  #change ACM number as found from ls /dev/tty/ACM*
ser.baudrate=9600

@app.route('/', methods=['GET'])
def main_post():
    cmd = request.args.get('cmd', None)
    if (isValidCMD(cmd)):
        ser.write(cmd)
    else:
        pass
        # do nothing

if __name__ == '__main__':
    app.run(debug=True, host='0.0.0.0', port=314)
