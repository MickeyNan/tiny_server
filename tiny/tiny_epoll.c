#include "csapp.h"
#include <sys/epoll.h>

#define LISTENQ 1024
#define SA struct sockaddr_in


int main(int argc,char **argv){
	int sockfd,connfd,listenfd,port,maxi,epfd,nfd;
	struct sockaddr_in servaddr,cliaddr;
	socklen_t clilen;
	struct epoll_event ev,event_polls[256];

	bzero(&servaddr,sizeof(servaddr));
	clilen = sizeof(cliaddr);
	
	if (argc != 2)
		unix_error("Arguments error,you need a port!");
	port = atoi(argv[1]);
	
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(atoi);
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	
	listenfd = Socket(AF_INET,SOCK_STREAM,0);
	
	epfd = epoll_create(256);
	ev.data.fd = listenfd;
	ev.events = EPOLLET|EPOLLIN;

	epoll_ctl(epfd,EPOLL_CTL_ADD,listenfd,&ev);
	
	Bind(listenfd,(SA *)&servaddr,sizeof(servaddr));
	Listen(listenfd,LISTENQ);
	
	for(;;){
		nfd = epoll_wait(epfd,event_polls,256,-1);
		int i = 0;
		for (;i < nfd;i++){
			if (event_polls[i].data.fd == listenfd){
				connfd = Accept(listenfd,(SA *)&cliaddr,&clilen);
				//doit;
				ev.data.fd = connfd;
				epoll_ctl(epfd,EPOLL_CTL_ADD,connfd,&ev);
			}
			else if (event_polls[i].events & EPOLLIN){
				
			}
			
			
		}
		
		
	
	
	}
	
	

}