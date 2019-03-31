from flask import Flask, request
# from flask_jsonpify import jsonify
from utils import *
import requests
import serial
import time
import threading

static_dir = "static"

app = Flask(__name__, static_folder=static_dir)

# All Arduino related content are marked for further test with hardware

ser = serial.Serial("/dev/ttyACM0",115200)  #change ACM number as found from ls /dev/tty/ACM*
ser.baudrate=115200

@app.route('/getCMD', methods=['POST'])
def main_post():
    global cmd
    cmd = request.get_json(force=True)['cmd']
    print ('data from client:', cmd)
    if (True):
        # i = 1
        # while(i == 1):
        #     time.sleep(1)
        #     i = send_command()
        # takeBuffer()
        send_command()
        # all cmd completes
        # ser.write(bytes('A'))
        # print('A sent')
        return "VALID CMD"
    else:
        return "INVALID CMD"

def send_alert(status):
    dictToSend = {'alert':status}
    res = requests.post('http://rudedski.inf.ed.ac.uk:1337/getAlert', json=dictToSend)
    print ('response from server:', res.text)

def resetCMD():
    global cmd
    cmd = ['AAA']
    print('reset ok!')

def send_command():
    global cmd
    print('before',cmd)
    # if len(cmd) == 0:
    #     print(cmd)
    #     return
    if cmd[0][0] != []:
        mode = 'transition'
        cur_cmd = cmd[0][0][0][0]
        del cmd[0][0][0]
        ser.write(bytes(cur_cmd))
        print('doing',cur_cmd,mode)
        print('after',cmd)
        return 1
    elif cmd[0][1] != []:
        mode = 'execution'
        cur_cmd = cmd[0][1][0][0]
        del cmd[0][1][0]
        ser.write(bytes(cur_cmd))
        print('doing',cur_cmd,mode)
        print('after',cmd)
        return 1
    else:
        print ('complete',cmd)
        cmd = cmd[1:]
        if len(cmd) >0:
            changeCurRequest(cmd[0][2])
            ser.write(bytes('P'))
            return 1
        else:
            print ('complete all cmd')
            removeCurRequest('none')
            takeBuffer()
            return 0

def changeCurRequest(id):
    dictToSend = {'id':id}
    res = requests.post('http://rudedski.inf.ed.ac.uk:1337/changeCurRequest', json=dictToSend)
    print ('response from server:', res.text)
def removeCurRequest(id):
    dictToSend = {'id':id}
    res = requests.post('http://rudedski.inf.ed.ac.uk:1337/removeCurRequest', json=dictToSend)
    print ('response from server:', res.text)

def takeBuffer():
    dictToSend = {'takeBuffer':'True'}
    res = requests.post('http://rudedski.inf.ed.ac.uk:1337/takeBuffer', json=dictToSend)
    print ('response from server:', res.text)

def readSerial():
    global cmd
    while (True):
    # NB: for PySerial v3.0 or later, use property `in_waiting` instead of function `inWaiting()` below!
        # ser.flushInput()
        if (ser.inWaiting()>0): #if incoming bytes are waiting to be read from the serial input buffer
            data_str = ser.read(ser.inWaiting()).decode('ascii') #read the bytes and convert from binary array to ASCII
            print(data_str) #print the incoming string without putting a new-line ('\n') automatically after every print()
            print("::::::::")
            # if (data_str=='O'):
            #     send_alert('True')
            # if (data_str=='c'):
            #     send_alert('False')
            if (data_str=='n'):
                print(cmd)
                send_command()
                print("??????")
            print("123123123",data_str)

 #Put the rest of your code you want here
        time.sleep(0.01)

if __name__ == '__main__':
    thread = threading.Thread(target=readSerial)
    thread.start()
    # resetCMD()
    app.run(debug=False, host='patamon.inf.ed.ac.uk', port=3142)
