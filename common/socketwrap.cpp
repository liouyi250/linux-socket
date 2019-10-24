#include "socketwrap.h"
#include <stdio.h>

int Socket(int domain,int type,int protocol){
	int connfd=socket(domain,type,protocol);
	if(connfd==-1){
		printf("socket error!\n");
	}
	return connfd;
}

int Listen(int sockfd,int backlog){
	int n=listen(sockfd,backlog);
	if(n==-1){
		printf("listen error!\n");
	}
	return n;
}

int Bind(int sockfd,const struct sockaddr *addr,socklen_t addrlen){
	int n=bind(sockfd,addr,addrlen);
	if(n==-1){
		printf("bind error!\n");
	}
	return n;
}

int Accept(int sockfd,struct sockaddr *addr,socklen_t *addrlen){
	int n=accept(sockfd,addr,addrlen);
	if(n==-1){
		printf("accept error!\n");
	}
	return n;
}

int Close(int fd){
	int n=close(fd);
	if(n==-1){
		printf("close error!\n");
	}
	return n;
}

int Connect(int sockfd,const struct sockaddr *addr,socklen_t addrlen){
	int n=connect(sockfd,addr,addrlen);
	if(n==-1){
		printf("connect error!\n");
	}
	return n;
}

int Read(int fd,void *buf,size_t count){
	int n=read(fd,buf,count);
	if(n==-1){
		printf("read error!\n");
	}
	return n; //the number of bytes
}

int Write(int fd, const void *buf,size_t count){
	int n=write(fd,buf,count);
	if(n==-1){
		printf("write error!\n");
	}
	return n;
}

int Recv(int sockfd,void *buf,size_t len,int flags){
	int n=recv(sockfd,buf,len,flags);
	if(n==-1){
		printf("recv error!\n");
	}
	return n;
}

int Send(int sockfd,const void *buf,size_t len,int flags){
	int n=send(sockfd,buf,len,flags);
	if(n==-1){
		printf("send error!\n");
	}
	return n;
}

int Select(int nfds,fd_set *readfds,fd_set *writefds,fd_set *exceptfds,struct timeval *timeout){
	int n=select(nfds,readfds,writefds,exceptfds,timeout);
	if(n==-1){
		printf("select error!\n");
	}
	return n;
}
