#! /usr/bin/python
import threading, time, httplib, random
# ��Ҫ���Ե� url �б�ÿһ�εķ��ʣ��������ȡһ��
urls = [
 "/test?page=",
 "/test2?orderby=a&page=",
 "/test2?orderby=d&page=",
]
MAX_PAGE = 10000
SERVER_NAME = "192.168.154.132:80"
TEST_COUNT = 10000
# ����һ�� threading.Thread ��������
class RequestThread(threading.Thread):
 # ���캯��
 def __init__(self, thread_name):
  threading.Thread.__init__(self)
  self.test_count = 0

 # �߳����е���ں���
 def run(self):
  # ��ֱ�ӰѴ���д��run��������ΪҲ�����ǻ�Ҫ��������ʽ�Ĳ���
  i = 0
  while i < TEST_COUNT:
   self.test_performace()
   i += 1
  #self.test_other_things()

 def test_performace(self):
  conn = httplib.HTTPConnection(SERVER_NAME)
  # ģ�� Keep-Alive �ķ���, HTTP 1.1
  for i in range(0, random.randint(0, 100)):
   # ����һ�� url���ṩ�������������
   url = urls[random.randint(0, len(urls) - 1)]
   url += str(random.randint(0, MAX_PAGE))
   # ������ӵ���������ȥ
   #print url
   try:
    conn.request("GET", url)
    rsps = conn.getresponse()
    if rsps.status == 200:
     # ��ȡ���ص�����
     data = rsps.read()
    self.test_count += 1
   except:
    continue
   
  conn.close()
  
# main ���뿪ʼ

# ��ʼ��ʱ��
start_time = time.time()
threads = []
# �������߳���
thread_count = 100

i = 0
while i < thread_count:
 t = RequestThread("thread" + str(i))
 threads.append(t)
 t.start()
 i += 1
# ����ͳ�Ƶ�����
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
  # ׼���˳� ��ʵ X �� ���ڸ������ף�ûʲô�˷ѵ���Դ
  TEST_COUNT = 0
  for t in threads:
   t.join(0)
  break