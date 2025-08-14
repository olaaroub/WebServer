import socket
import time

host = '127.0.0.1'
port = 8080
#/ HTTP/1.1\r\nHost: localhost\r\nContent-Length: 10\r\n\r\n
# request with Content-Length
RequestLineBuf1 = b"POST "
RequestLineBuf2 = b"/ "
RequestLineBuf3 = b"HTTP/1.1\r\n"

HeadersBuff1 = b"Host: localhost\r\n"
HeadersBuff2 = b"Content-Length: 100\r\n\r\n"

req2 = b"12345"
req3 = b"67890"

sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
sock.connect((host, port))

print("send RequestLineBuf1")
sock.sendall(RequestLineBuf1)
time.sleep(0.1)

print("send RequestLineBuf2")
sock.sendall(RequestLineBuf2)
time.sleep(2)


print("send RequestLineBuf3")
sock.sendall(RequestLineBuf3)
time.sleep(0.1)

print("send HeadersBuff1")
sock.sendall(HeadersBuff1)
time.sleep(0.1)

print("send HeadersBuff2")
sock.sendall(HeadersBuff2)
time.sleep(0.1)

# for c in HeadersBuff2:
#     sock.sendall(bytes([c]))
#     time.sleep(0.1)

print("send req2")
sock.sendall(req2)
time.sleep(0.1)

print("send req3")
sock.sendall(req3)

time.sleep(0.1)
response = sock.recv(4096)
print("response:", response.decode(errors='ignore'))

sock.close()