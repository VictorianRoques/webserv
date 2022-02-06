import requests
import socket
import time

def stress_tester():
	socks = []
	for i in range(1500):
		time.sleep(0.005)
		socks.append(socket.socket(socket.AF_INET, socket.SOCK_STREAM))
		socks[i].connect(('localhost', 8080))
		socks[i].settimeout(3)
		socks[i].send(b'GET / HTTP/1.1\r\n\
Host: localhost:8080\r\n\
User-Agent: python-requests/2.27.1\r\n\
Accept-Encoding: gzip, deflate\r\n\
Accept: */*\r\n\
Connection: keep-alive\r\n\r\n')
		try:
			socks[i].recv(2048)
			strg = 'socket ' + str(i) + ': data received from server'
			print(strg)
		except Exception as e:
			strg = 'socket ' + str(i) + ': cannot receive data from server'
			print(strg)

def chunk_tester():
	sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
	sock.connect(('localhost', 8080))
	sock.send(b'POST / HTTP/1.1\r\n\
Host: localhost:8080\r\n\
User-Agent: python-requests/2.27.1\r\n\
Accept-Encoding: gzip, deflate\r\n\
Accept: */*\r\n\
Connection: keep-alive\r\n\
Transfer-Encoding: chunked\r\n\r\n')
	time.sleep(0.1)
	sock.send(b'C\r\n\
Hello World!\r\n\r\n')
	time.sleep(0.1)
	sock.send(b'33\r\n\
This is another part of a very boring chunk request\r\n\r\n')
	time.sleep(0.1)
	sock.send(b'38\r\n\
And finally the last third part of a chunk dummy request\r\n\r\n')
	time.sleep(0.1)
	sock.send(b'0\r\n\r\n')
	tmp = sock.recv(2048)
	print(tmp)

print('########## STRESS_TESTER ##########')
stress_tester()

print('\n########## CHUNK_TESTER ##########')
chunk_tester()
