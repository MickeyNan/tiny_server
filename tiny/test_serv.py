#! /usr/bin/python
import threading,time,httplib,random

urls = [
	"/home.html",
	"/godzilla.gif",
]

#MAX_PAGE = 10000
SERVER_NAME = "192.168.154.132:80"
TEST_COUNT = 10000


class TestThread(threading.Thread):
	def __init__(self,threadname):
		threading.Thread.__init__(self)
		self.test_count = 0
		self.resp_time = 0
		
	def run(self):
		i = 0
		while i < TEST_COUNT:
			self.test_perform()
			i += 1
		
	def test_perform(self):
		for i in range(0,random.randint(0,100)):
			url = urls[random.randint(0,len(urls) - 1)]
			#
			try:
				send_time = time.time()
				conn = httplib.HTTPConnection(SERVER_NAME)
				conn.request("GET",url)
				resp = conn.getresponse()
				if resp.status == 200: 
				 data = resp.read()
				self.resp_time += time.time() - send_time
				self.test_count += 1
			except:
				continue


start_time = time.time()
threads = []
thread_count = 100

i = 0
while i < thread_count:
	t = TestThread("thread" + str(i))
	threads.append(t)
	t.start()
	i += 1

	
word = ""
while True:
	word = raw_input("cmd:")
	if word == "s":
		time_span = time.time() - start_time
		all_count = 0
		all_resp_time = 0
		for t in threads:
			all_count += t.test_count
			all_resp_time += t.resp_time
	 
		print "%s Request/Second " % str(all_count / time_span) 
		print "%s Response time" % str(all_resp_time / all_count)
	elif word == "e":
  
		TEST_COUNT = 0
		for t in threads:
			t.join(0)
		break

