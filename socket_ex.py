
from time import sleep
import socket
import re

from enum import Enum
class Status(Enum):
    MOVING = 1
    DONE = 2
    ABORTED = 3
    ERROR = 4

class SES_API:
    def __init__(self):
        self.HOST = "127.0.0.1"  # Standard loopback interface address (localhost)
        self.PORT = 5000  # Port to listen on (non-privileged ports are > 1023)
        self.status =  Status.DONE
        self.conn = None
        self.pos = {"P":3.14,"tilt":2.14}
        self.move_reg = re.compile('(X|Y|Z|P|T|F)([+-]?([0-9]*[.])?[0-9]+)') #capturing X or Y or Z and float number
        self.pos_reg = re.compile('(X|Y|Z|P|T|F)(\?)') #capturing X or Y or Z and float number
        #P - polar T- tilt  F - phi
    
    def move(self,data):
        self.status = Status.MOVING
        m= self.move_reg.match(data.decode("UTF-8"))
        axis, pos  = m.group(0), m.group(1)
        print(axis,pos)
        # here we want to run a thread that keeps checking if position is arrived. Maybe not here but in the main
        # motor script!!
        # but, then set this class to DONE.
        
        
        #DO MOVE!
        
        
    def send_pos(self,data):
         axis = self.pos_reg.match(data.decode("UTF-8")).group(0)
         #get number
         self.conn.send("2.17\n".encode())
         
    def stop(self):
        pass
    
    def send_status(self):
        print('send status')
        self.conn.send("{}\n".format(self.status.value).encode())
    
    def handle_req(self,data):
        if "STATUS" in data.decode("UTF-8"):
            self.send_status()
        else:
            self.send_pos(data)
    
    def handle_connection(self):#this is main loop.
        with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
            s.bind((self.HOST, self.PORT))
            s.listen()
            print("listening")
            while True:
                self.conn, addr = s.accept()
                with self.conn:
                    print("Connected by {}".format(addr))
                    while True:
                        data = self.conn.recv(512)
                        if data==b'':
                            continue
                        if("?" in data.decode("UTF-8")):
                            self.handle_req(data) #Handle data request
                        elif "MOV" in data.decode("UTF-8"): #MOVX5.0 for example
                            self.move(data) #handle move request
                        elif "STOP" in data.decode("UTF-8"):
                            self.stop()
                        else:
                            print(data) # anything else pleas?
                        
                    if data.decode("UTF-8")=="exit":
                        # closing connection, but awaiting another one...
                        break
                    sleep(0.1)
                    
                sleep(0.1)
            

api = SES_API()
api.handle_connection()