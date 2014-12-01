#include "csapp.h"

void send_error(int fd,char *status,char *status_info,char *mime,int length);
void send_header(int fd,char *status,char *status_info,char *mime,int length);
void read_request(rio_t *rio);
int parse_uri(char *uri,char *filename,char *cgiargs);
void serv_static(int fd,char *filename,int filesize);
void serv_dynamic(int fd,char *filename,char *args);
void get_file_type(char *filename,char *filetype);
int file_details(char *filename);
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
		send_error(connfd,"501","Method Not Implemented","text/html",25);
		return;
	}	
	
	read_request(&rio);
	
	is_static = parse_uri(uri,filename,cgiargs);
	if (stat(filename,&st) < 0){
		send_error(connfd,"404","File Not Found","text/html",25);
		return ;
	}
	if(is_static){
		if (!S_ISREG(st.st_mode)||!(S_IRUSR & st.st_mode)){
			send_error(connfd,"403","Forbidden","text/html",25);
			return ;
		}	
		serv_static(connfd,filename,st.st_size);
	}
	else if(is_static == 0){
		if (!S_ISREG(st.st_mode)||!(S_IXUSR & st.st_mode)){
			send_error(connfd,"403","Forbidden","text/html",25);
			return ;
		}
		serv_dynamic(connfd,filename,cgiargs);
	}
	
}

void send_error(int fd,char *status,char *status_info,char *mime,int length){
	char body[1024];
	char head[1024];
	sprintf(body,"<html><title>Tiny_Server Error!</title>");
	sprintf(body,"%s<body bgcolor = ""ffffffff""></body>\r\n",body);
	sprintf(body,"%s%s: %s\r\n",body,status,status_info);
	sprintf(body,"%s<hr><em>Tiny_server~</em>\r\n",body);
	send_header(fd,status,status_info,mime,length);
	Rio_writen(fd,body,strlen(body));
}

void send_header(int fd,char *status,char *status_info,char *mime,int length){
	char head[1024];
	sprintf(head,"HTTP/1.0 %s %s\r\n",status,status_info);
	Rio_writen(fd,head,strlen(head));
	sprintf(head,"Content-type: %s\r\n",mime);
	Rio_writen(fd,head,strlen(head));
	sprintf(head,"Content-length: %d\r\n",length);
	Rio_writen(fd,head,strlen(head));
}

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
		if (uri[strlen(uri - 1)] == '/'){
			strcat(filename,"home.html");
		}
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

void serv_static(int fd,char *filename,int filesize){
	char *ptr,filetype[MAXLINE],head[MAXLINE];
	int srcfd;
	get_file_type(filename,filetype);
	
	sprintf(head,"HTTP/1.0 200 OK\r\n");
	sprintf(head,"%sSever: Tiny_Server\r\n",head);
	sprintf(head,"%sConten-length:%d\r\n",head,filesize);
	sprintf(head,"%sContent-type:%s\r\n",head,filetype);
	Rio_writen(fd,head,strlen(head));
	
	srcfd = Open(filename,O_RDONLY,0);
	ptr = mmap(0,filesize,PROT_READ,MAP_PRIVATE,srcfd,0);
	Close(srcfd);
	Rio_writen(fd,ptr,sizeof(ptr));
	Munmap(ptr,filesize);
	return ;
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
	else if (".gif")
		strcpy(filetype,"image/gif");
	else if (".jpg")
		strcpy(filetype,"image/jpeg");
	else
		strcpy(filetype,"text/plain");

}