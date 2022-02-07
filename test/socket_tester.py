#!/usr/bin/python

import socket
import time
import threading

lock = threading.Lock()

def stress_tester1():
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
			lock.acquire()
			print("                                                  ", end='\r')
			print(strg, end='\r')
			lock.release()
		except:
			strg = 'socket ' + str(i) + ': cannot receive data from server'
			print("                                                  ", end='\r')
			print(strg, end='\r')

def stress_tester2():
	try:
		thread1 = threading.Thread(target=stress_tester1)
		thread2 = threading.Thread(target=stress_tester1)
		print("Thread 1 starting...")
		thread1.start()
		time.sleep(2)
		lock.acquire()
		print("                                                  ", end='\r')
		print("Thread 2 starting...")
		lock.release()
		thread2.start()
		thread1.join()
		thread2.join()
	except:
		print("error: threads failure")
		exit()

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

def memory_tester():
	sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
	sock.connect(('localhost', 8080))
	sock.send(b'POST / HTTP/1.1\r\n\
Host: localhost:8080\r\n\
User-Agent: python-requests/2.27.1\r\n\
Accept-Encoding: gzip, deflate\r\n\
Accept: */*\r\n\
Connection: keep-alive\r\n\
Transfer-Encoding: chunked\r\n\r\n')
	for i in range(3965 * 8 + 2):
		time.sleep(0.001)
		sock.send(b'3E8\r\n\
Phasellus consequat placerat tristique. Fusce vel eleifend tortor. Aenean egestas non risus et vehicula. Nam ac mauris odio. Maecenas convallis mi vel justo vehicula, a interdum leo porta. Mauris consequat nec urna sed aliquet. Aliquam aliquet, dui id hendrerit auctor, turpis ante facilisis sapien, sit amet interdum nibh ipsum et ex. Etiam vulputate in libero quis viverra. Nunc sit amet ipsum id ipsum scelerisque efficitur. Vivamus in sapien id tortor dapibus accumsan. Cras eleifend, est et vestibulum efficitur, lorem velit pulvinar mi, nec vehicula ligula nisl quis tortor. In sagittis sapien sit amet enim rutrum varius. Maecenas cursus justo a sapien blandit, sit amet tincidunt sem lacinia. Suspendisse bibendum tristique orci, et ornare massa mollis sit amet. Aenean dictum, turpis vitae varius auctor, arcu mi malesuada libero, et ullamcorper dolor justo in turpis. Pellentesque vestibulum porttitor porta. Phasellus consequat placerat tristique. Phasellus consequat placerat tristique...\r\n\r\n')
	sock.send(b'0\r\n\r\n')
	ret = sock.recv(2048)
	print(ret)

# print('\n------------------------------------------')
# print('          STRESS_TESTER_1 starting...')
# print('------------------------------------------')
# time.sleep(1.5)
# stress_tester1()
# print('\n\nSTRESS_TESTER_1 [ \033[0;32mOK\033[0m ]\n')
print('\n------------------------------------------')
print('          STRESS_TESTER _2 starting...')
print('------------------------------------------')
time.sleep(1.5)
stress_tester2()
print('\n\nSTRESS_TESTER_2 [ \033[0;32mOK\033[0m ]\n')
# print('\n------------------------------------------')
# print('          CHUNK_TESTER starting...')
# print('------------------------------------------')
# time.sleep(1.5)
# chunk_tester()
# print('\nCHUNK_TESTER [ \033[0;32mOK\033[0m ]')
# print('\n------------------------------------------')
# print('          MEMORY_TESTER starting...')
# print('------------------------------------------')
# time.sleep(1.5)
# memory_tester()
# print('\nMEMORY_TESTER [ \033[0;32mOK\033[0m ]')
