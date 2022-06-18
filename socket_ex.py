
from time import sleep
import socket

HOST = "127.0.0.1"  # Standard loopback interface address (localhost)
PORT = 5000  # Port to listen on (non-privileged ports are > 1023)

with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
    s.bind((HOST, PORT))
    s.listen()
    print("listening")
    conn, addr = s.accept()
    with conn:
        print("Connected by {}".format(addr))
        while True:
            data = conn.recv(512)
            if data==b'':
                continue
            if(data.decode("UTF-8")  =="X?"):
                conn.send(("3.141\n").encode())
            elif data.decode("UTF-8")=="Y?":
                conn.send("2.17\n".encode())
            else:
                print(data)
                
            if data.decode("UTF-8")=="exit":
                break
            sleep(0.1)
            
         