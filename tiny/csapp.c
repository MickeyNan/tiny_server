#include "csapp.h"

#define SA struct sockaddr_in
#define bzero(ptr,n) memset(ptr,0,n)
void unix_error(char *msg){
	fprintf(stderr,"%s:%s\n",msg,strerror(errno));
	exit(0);
}

void posix_error(int code, char *msg){
	fprintf(stderr,"%s:%s\n",msg,strerror(code));
	exit(0);
}

pid_t Fork(void){
	pid_t pid;
	if ((pid = fork()) < 0){
		unix_error("Fork error!");
	}
	return pid;
}

void Execve(const char *filename, char *const argv[], char *const envp[]){
	if (execve(filename,argv,envp) < 0)
		unix_error("Execve erro!");
}

pid_t Wait(int *status){
	pid_t pid;
	if ( (pid = wait(status)) < 0)
		unix_error("Wait error!");
	return pid;
}

pid_t Waitpid(pid_t pid, int *iptr, int options){
	pid_t w_pid;
	if ( (w_pid = waitpid(pid,iptr,options)) < 0)
		unix_error("Waitpid error!");
	return w_pid;
}

void Kill(pid_t pid, int signum){
	if (kill(pid,signum) < 0)
		unix_error("Kill error!");
}

unsigned int Sleep(unsigned int secs){
	unsigned int left_sec;
	if( (left_sec = sleep(secs)))
		unix_error("Sleep error!");
	return left_sec;
}

void Pause(void){
	(void)pause();
	return;
}

unsigned int Alarm(unsigned int seconds){
	return alarm(seconds);
}

void Setpgid(pid_t pid, pid_t pgid){
	if(setpgid(pid,pgid))
		unix_error("Setpgid error!");
}

pid_t Getpgrp(){
	return getpgrp();
}

handler_t *Signal(int signum, handler_t *handler){
    struct sigaction action, old_action;

    action.sa_handler = handler;  
    sigemptyset(&action.sa_mask); /* block sigs of type being handled */
    action.sa_flags = SA_RESTART; /* restart syscalls if possible */

    if (sigaction(signum, &action, &old_action) < 0)
	unix_error("Signal error");
    return (old_action.sa_handler);
}

void Sigprocmask(int how, const sigset_t *set, sigset_t *oldset){
	if (sigprocmask(how,set,oldset) < 0)
		unix_error("Sigpromask error!");
	return ;
}

void Sigemptyset(sigset_t *set){
	if (sigemptyset(set) < 0)
		unix_error("Empty the sigset error!");
	return;
}

void Sigfillset(sigset_t *set){
	if (sigfillset(set) < 0)
		unix_error("Fill the set error!");
	return ;
}

void Sigaddset(sigset_t *set, int signum){
	if (sigaddset(set,signum) < 0)
		unix_error("Add sig to set error!");
	return ;
}	

void Sigdelset(sigset_t *set, int signum){
	if (sigdelset(set,signum) < 0)
		unix_error("Delete signum from set error!");
	return ;
}

int Sigismember(const sigset_t *set, int signum){
	int judge;
	if ((judge = sigismember(set,signum)) < 0)
		unix_error("Judge signum from set error!");
	else 
		return judge;	
}

int Open(const char *pathname, int flags, mode_t mode){
	int rc;
	if ( (rc = open(pathname,flags,mode)) < 0)
		unix_error("Open file error!");
	return rc;
	
}

ssize_t Read(int fd, void *buf, size_t count){
	ssize_t rc;
	if ((rc = read(fd,buf,count)) < 0){
		unix_error("Read from fd error!");
	}
	return rc;
}

ssize_t Write(int fd, const void *buf, size_t count){
	ssize_t rc;
	if (write(fd,buf,count))
		unix_error("Write error!");
	return rc;
}

off_t Lseek(int fildes, off_t offset, int whence){
	off_t rc;
	if ((rc = lseek(fildes,offset,whence)) < 0)
		unix_error("Lseek error!");
	return rc;
}

void Close(int fd){
	if (close(fd) < 0)
		unix_error("Close error!");
	return ;
}

int Select(int  n, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, struct timeval *timeout){
	int rc;
	//n represents the maximum  fd 
	if ( (rc = select(n,readfds,writefds,exceptfds,timeout)) < 0)
		unix_error("Select error!");
	
	return rc;
}

int Dup2(int fd1, int fd2){
	int rc;
	if ( (rc = dup2(fd1,fd2)) < 0)
		unix_error("Dup2 error!");
	return rc;
}

void Stat(const char *filename, struct stat *buf){
	if (stat(filename,buf) < 0)
		unix_error("Stat error!");
}

void Fstat(int fd, struct stat *buf){
	if (fstat(fd,buf) < 0)
		unix_error("Fstat error!");
}

void *Mmap(void *addr, size_t len, int prot, int flags, int fd, off_t offset){
	void *ptr;
	if ((ptr = mmap(addr,len,prot,flags,fd,offset)) == ((void *)-1))
		unix_error("Mmap error!");
	return ptr;
}

void Munmap(void *start, size_t length){
	if (munmap(start,length) < 0)
		unix_error("Munmap error!");
}

void *Malloc(size_t size){
	void *ptr;
	if ((ptr = malloc(size)) == NULL)
		unix_error("malloc error!");
}		

void *Realloc(void *ptr, size_t size){
	void *p;
	if ((p = realloc(ptr,size)) == NULL)
		unix_error("Re error!");
	return p;
}

void *Calloc(size_t nmemb, size_t size){
	void *p;
	if ( (p = calloc(nmemb,size)) == NULL)
		unix_error("Calloc error!");
	return p;
}

void Free(void *ptr){
	free(ptr);
}

int Socket(int domain, int type, int protocol){
	int sockfd;
	if ((sockfd = socket(domain,type,protocol)) < 0)
		unix_error("Socket error!");
	return sockfd;
}

void Setsockopt(int s, int level, int optname, const void *optval, int optlen){
	if (setsockopt(s,level,optname,optval,optlen) < 0)
		unix_error("Setsockopt error!");
	return ;
}

void Bind(int sockfd, struct sockaddr *my_addr, int addrlen){
	if (bind(sockfd,my_addr,addrlen) < 0)
		unix_error("Bind error!");
}

void Listen(int s, int backlog){
	if (listen(s,backlog) < 0)
		unix_error("Listen error!");
	return ;
}

int Accept(int s, struct sockaddr *addr, socklen_t *addrlen){
	int connfd;
	if ( (connfd = accept(s,addr,addrlen)) < 0)
		unix_error("Accept error!");
	return connfd;
}

void Connect(int sockfd, struct sockaddr *serv_addr, int addrlen){
	if (connect(sockfd,serv_addr,addrlen) < 0)
		unix_error("Connect error!");
	return;
}

ssize_t rio_readn(int fd, void *usrbuf, size_t n){
	size_t left = n;
	ssize_t have_read;
	void *ptr = usrbuf;
	while(1){
		if ((have_read = read(fd,usrbuf,left)) < 0){
			if (errno == EINTR)
				have_read = 0;
			else	
				return -1;
		}
		else if (have_read == 0)//EOF
			break;
		left -= have_read;
		ptr += have_read;
	}
	return (n - left);
}

ssize_t rio_writen(int fd, void *usrbuf, size_t n) 
{
    size_t nleft = n;
    ssize_t nwritten;
    char *bufp = usrbuf;

    while (nleft > 0) {
	if ((nwritten = write(fd, bufp, nleft)) <= 0) {
	    if (errno == EINTR)  /* interrupted by sig handler return */
		nwritten = 0;    /* and call write() again */
	    else
		return -1;       /* errorno set by write() */
	}
	nleft -= nwritten;
	bufp += nwritten;
    }
    return n;
}
void rio_readinitb(rio_t *rp, int fd){
    rp->rio_fd = fd;  
    rp->rio_cnt = 0;  
    rp->rio_bufptr = rp->rio_buf;

}

ssize_t	rio_readnb(rio_t *rp, void *usrbuf, size_t n){
	size_t nleft = n;
    ssize_t nread;
    char *bufp = usrbuf;
    
    while (nleft > 0) {
	if ((nread = rio_readn(rp->rio_fd, (void *)bufp, nleft)) < 0) {
	    if (errno == EINTR) /* interrupted by sig handler return */
		nread = 0;      /* call read() again */
	    else
		return -1;      /* errno set by read() */ 
	} 
	else if (nread == 0)
	    break;              /* EOF */
	nleft -= nread;
	bufp += nread;
    }
    return (n - nleft);     
}

ssize_t	rio_readlineb(rio_t *rp, void *usrbuf, size_t maxlen){
    int n, rc;
    char c, *bufp = usrbuf;

    for (n = 1; n < maxlen; n++) { 
	if ((rc = rio_readn(rp->rio_fd, &c, 1)) == 1) {
	    *bufp++ = c;
	    if (c == '\n')
		break;
	} else if (rc == 0) {
	    if (n == 1)
		return 0; /* EOF, no data read */
	    else
		break;    /* EOF, some data was read */
	} else
	    return -1;	  /* error */
    }
    *bufp = 0;
    return n;

}

ssize_t Rio_readn(int fd, void *usrbuf, size_t n){
	ssize_t have_read;
	if ((have_read = rio_readn(fd,usrbuf,n)) < 0)
		unix_error("Rio_read error!");
	
	return have_read;
}

void Rio_writen(int fd, void *usrbuf, size_t n){
	if (rio_writen(fd,usrbuf,n) != n)
		unix_error("Rio_writen error!");
	return ;
}

void Rio_readinitb(rio_t *rp, int fd)
{
    rio_readinitb(rp, fd);
} 

ssize_t Rio_readnb(rio_t *rp, void *usrbuf, size_t n) 
{
    ssize_t rc;

    if ((rc = rio_readnb(rp, usrbuf, n)) < 0)
		unix_error("Rio_readnb error");
    return rc;
}

ssize_t Rio_readlineb(rio_t *rp, void *usrbuf, size_t maxlen){
	ssize_t have_read;
	if ( (have_read = rio_readlineb(rp,usrbuf,maxlen)) < 0)
		unix_error("Rio_realineb error!");
	
	return have_read;
}

int open_clientfd(char *hostname, int portno){
	
}

int open_listenfd(int portno){
	int listenfd;
	int optval=1;
	struct sockaddr_in servaddr;
	listenfd = Socket(AF_INET,SOCK_STREAM,0);
	Setsockopt(listenfd,SOL_SOCKET,SO_REUSEADDR,(const void *)&optval,sizeof(int));
	
	bzero(&servaddr,sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons((unsigned short)portno);
	
	Bind(listenfd,(SA *)&servaddr,sizeof(servaddr));
	
	Listen(listenfd,1024);
	return listenfd;
}

int Open_listenfd(int port) 
{
    int rc;

    if ((rc = open_listenfd(port)) < 0)
	unix_error("Open_listenfd error");
    return rc;
}