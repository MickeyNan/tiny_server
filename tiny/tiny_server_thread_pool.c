#include "csapp.h"

#define SA struct sockaddr

void send_error(int fd,char *status,char *status_info,char *mime);
//void send_header(int fd,char *status,char *status_info,char *mime,int length);
void read_request(rio_t *rio);
int parse_uri(char *uri,char *filename,char *cgiargs);
void serv_static(int fd,char *filename,int filesize);
void serv_dynamic(int fd,char *filename,char *args);
void get_file_type(char *filename,char *filetype);
int file_details(char *filename);
void doit(int connfd);
void sig_child(int signo);//用作信号处理
void *thread_do(void *arg);
void thread_make(int i);

typedef struct{
	pthread_t thread_tid;
	long thread_count;
}Thread;
Thread *tptr;

pthread_mutex_t m_lock = PTHREAD_MUTEX_INITIALIZER;
int cli_len;
int listenfd;
int main(int argc,char **argv){
	int connfd;
	int nthreads = 100;//预先派生的线程数S
	struct sockaddr_in servaddr;
	struct sockaddr_in cliaddr;
	cli_len = sizeof(cliaddr);
	int port;
	if(argc != 2){
		printf("You shall input the port !\n");
		exit(0);
	}
		
	//listenfd = Socket(AF_INET,SOCK_STREAM,0);
	port = atoi(argv[1]);
	listenfd = Open_listenfd(port);
	
	int i;//临时变量
	tptr = Calloc(nthreads,sizeof(Thread));//保存Thread的数组
	for(i = 0; i < nthreads; i++){
		thread_make(i);
	}
	
	for(;;){
	
	}
	

	
	
}

void doit(int connfd){
	char line[MAXLINE],method[MAXLINE],uri[MAXLINE],version[MAXLINE];
	char filename[MAXLINE],cgiargs[MAXLINE];
	struct stat st;
	int is_static;
	rio_t rio;
//ssize_t Rio_readlineb(rio_t *rp, void *usrbuf, size_t maxlen);
	
	Rio_readinitb(&rio,connfd);
	Rio_readlineb(&rio,line,MAXLINE);
	
	if (line != (char *)0)
		sscanf(line,"%s %s %s ",method,uri,version);
		
	if (strcasecmp(method,"get") != 0){
		//
		send_error(connfd,"501","Method Not Implemented","text/html");
		return;
	}	
	
	read_request(&rio);
	
	is_static = parse_uri(uri,filename,cgiargs);
	if (stat(filename,&st) < 0){
		send_error(connfd,"404","File Not Found","text/html");
		return ;
	}
	if(is_static){
		if (!S_ISREG(st.st_mode)||!(S_IRUSR & st.st_mode)){
			
			send_error(connfd,"403","Forbidden","text/html");
			return ;
		}	
		serv_static(connfd,filename,st.st_size);
	}
	else if(is_static == 0){
		if (!S_ISREG(st.st_mode)||!(S_IXUSR & st.st_mode)){
			send_error(connfd,"403","Forbidden","text/html");
			return ;
		}
		serv_dynamic(connfd,filename,cgiargs);
	}
	
}

void send_error(int fd,char *status,char *status_info,char *mime){
	char body[MAXLINE];
	char head[MAXLINE];
	sprintf(body,"<html><title>Tiny_Server Error!</title>");
	sprintf(body,"%s<body bgcolor = ""ffffffff""></body>\r\n",body);
	sprintf(body,"%s%s: %s\r\n",body,status,status_info);
	sprintf(body,"%s<hr><em>Tiny_server~</em>\r\n",body);
	
	sprintf(head,"HTTP/1.0 %s %s\r\n",status,status_info);
	Rio_writen(fd,head,strlen(head));
	sprintf(head,"Content-type: %s\r\n",mime);
	Rio_writen(fd,head,strlen(head));
	sprintf(head,"Content-length: %d\r\n\r\n",(int)strlen(body));
	Rio_writen(fd,head,strlen(head));
	Rio_writen(fd,body,strlen(body));
}

/*
void send_header(int fd,char *status,char *status_info,char *mime,int length){
	char head[1024];
	sprintf(head,"HTTP/1.0 %s %s\r\n",status,status_info);
	Rio_writen(fd,head,strlen(head));
	sprintf(head,"Content-type: %s\r\n",mime);
	Rio_writen(fd,head,strlen(head));
	sprintf(head,"Content-length: %d\r\n",length);
	Rio_writen(fd,head,strlen(head));
}
*/

void read_request(rio_t *rio){
	char buff[MAXLINE];
	Rio_readlineb(rio,buff,MAXLINE);
	while(strcmp(buff,"\r\n")){
		Rio_readlineb(rio,buff,MAXLINE);
		printf("%s\n",buff);//读取并将该报头忽略
	}
	return;
}

int parse_uri(char *uri,char *filename,char *cgiargs){
	char *ptr;
	if (!strstr(uri,"cgi-bin")){//Static page request
		strcpy(cgiargs,"");
		stpcpy(filename,".");
		strcat(filename,uri);
		if (uri[strlen(uri) - 1] == '/'){
			strcat(filename,"home.html");
		}
		printf("%s\n",filename);
		return 1;
	}
	else{//Dynamic page request
		ptr = index(uri,'?');
		if (ptr){
			strcpy(cgiargs,ptr + 1);
			*ptr = '\0';
		}
		else{
			strcpy(cgiargs,"");
			strcpy(filename,".");
			strcat(filename,uri);
			return 0;//代表动态网页
		}	
			
	}
	
}

void serv_static(int fd, char *filename, int filesize) 
{
    int srcfd;
    char *srcp, filetype[MAXLINE], buf[MAXBUF];
 
    /* Send response headers to client */
    get_file_type(filename, filetype);       //line:netp:servestatic:getfiletype
    sprintf(buf, "HTTP/1.0 200 OK\r\n");    //line:netp:servestatic:beginserve
    sprintf(buf, "%sServer: Tiny Web Server\r\n", buf);
    sprintf(buf, "%sContent-length: %d\r\n", buf, filesize);
    sprintf(buf, "%sContent-type: %s\r\n\r\n", buf, filetype);
    Rio_writen(fd, buf, strlen(buf));       //line:netp:servestatic:endserve

    /* Send response body to client */
    srcfd = Open(filename, O_RDONLY, 0);    //line:netp:servestatic:open
    //srcp = Mmap(0, filesize, PROT_READ, MAP_PRIVATE, srcfd, 0);//line:netp:servestatic:mmap
    srcp = (char *)malloc(filesize);
	Rio_readn(srcfd,srcp,filesize);
	Close(srcfd);                           //line:netp:servestatic:close
    Rio_writen(fd, srcp, filesize);         //line:netp:servestatic:write
    //Munmap(srcp, filesize);                 //line:netp:servestatic:munmap
}
void serv_dynamic(int fd,char *filename,char *args){
	char buf[MAXLINE],*emptylist[] = {NULL};
	sprintf(buf,"HTTP/1.0 200 OK\r\n");
	Rio_writen(fd,buf,strlen(buf));
	sprintf(buf,"Server:Tiny_Server\r\n");
	Rio_writen(fd,buf,strlen(buf));
	
	if (Fork() == 0){
		setenv("QUERY_STRING",args,1);
		Dup2(fd,STDOUT_FILENO);
		Execve(filename,emptylist,environ);
	}
	Wait(NULL);
}

void get_file_type(char *filename,char *filetype){
	if (strstr(filename,".html"))
		strcpy(filetype,"text/html");
	else if (strstr(filename,".gif"))
		strcpy(filetype,"image/gif");
	else if (strstr(filename,".gif"))
		strcpy(filetype,"image/jpeg");
	else if (strstr(filename,".mpg"))
		strcpy(filetype,"video/mpg");
	else if (strstr(filename,".mp3"))
		strcpy(filetype,"audio/mp3");
	else
		strcpy(filetype,"text/plain");

}

void sig_child(int signo){
	int pid;
	int status;
	while ( (pid = waitpid(-1,&status,WNOHANG)) > 0)//非阻塞的waitpid,就是告知内核在无子进程结束时不要阻塞
		printf("Child %d terminated\n",pid);
	return ;
}

void thread_make(int i){
	void *thread_do(void *);
	pthread_create(&tptr[i].thread_tid,NULL,&thread_do,(void *)i);
	return ;
}

void *thread_do(void *arg){
	void doit(int);
	int connfd;
	socklen_t clilen;
	struct sockaddr *cliaddr;
	printf("Thread %d starting\n",(int )arg);
	
	for(;;){
		clilen = cli_len;//?
		pthread_mutex_lock(&m_lock);
		connfd = Accept(listenfd,cliaddr,&clilen);
		pthread_mutex_unlock(&m_lock);
		tptr[(int)arg].thread_count++;
		
		doit(connfd);
		Close(connfd);
	}
}