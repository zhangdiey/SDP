from flask import Flask, request
from flask_jsonpify import jsonify
import serial
import time

static_dir = "static"

app = Flask(__name__, static_folder=static_dir)

ser=serial.Serial("/dev/ttyACM0",9600)  #change ACM number as found from ls /dev/tty/ACM*
ser.baudrate=9600

@app.route('/', methods=['GET'])
def main_post():
    cmd = request.args.get('cmd', None)
    if (cmd == "GO"):
        try:
            pass
            # send cmd to arduino
        except Exception as e:
            raise
    elif (cmd == "HALT"):
        try:
            pass
            # send cmd to arduino
        except Exception as e:
            raise
    else:
        pass
        # do nothing

if __name__ == '__main__':
    app.run(debug=True, host='0.0.0.0', port=1337)
