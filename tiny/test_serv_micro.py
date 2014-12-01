import threading,time,httplib,random

urls = {
	"/v2.html",
	"/microindex.html",
}

#MAX_PAGE = 10000
SERVER_NAME = "192.168.154.132:8000"
TEST_COUNT = 10000


class TestThread(threading.Thread):
	def __init__(self,threadname):
		threading.Thread.__init__(self)
		self.test_count = 0
		
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
				conn = httplib.HTTPConnect(SERVER_NAME)
				conn.request("GET",url)
				resp = conn.getesponse()
				if resp.status == 200: 
				 data = resp.read()
				self.test_count += 1
			except:
				print "connection error!"
				continue


start_time = time.time()
threads[]
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
	for t in threads:
		all_count += t.test_count
	print "%s Request/Second" % str(all_count / time_span)
	elif word == "e":
  # 准备退出 其实 X 掉 窗口更加容易，没什么浪费的资源
	TEST_COUNT = 0
	for t in threads:
		t.join(0)
	break

