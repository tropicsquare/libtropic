#!/usr/bin/python
#            # This is server.py file #Python3
import socket  # Import socket module

s = socket.socket(socket.AF_INET,socket.SOCK_STREAM)  # Create a socket object
s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)

host = "127.0.0.1" #socket.gethostname()  # Get local machine name
port = 12345  # Reserve a port for your service.
s.bind((host, port))  # Bind to the port

s.listen(5)  # Now wait for client connection.
c, addr = s.accept()  # Establish connection with client.
print('Got connection from', addr)

tries=3
while tries:
    tries -= 1
    a = input("\r\nTry the PIN:")
    c.sendall(bytes(a, "ascii"))
    received = c.recv(1024)
    if received == b'\x00':
        received = "LT_OK"
    else:
        received = "OTHER STATUS"
    print("status: " + str(received))

c.close()  # Close the connection