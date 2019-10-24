#include "./common/socketwrap.h"
#include <string.h>
#include <cstdio>
#include <arpa/inet.h>

int sendNewUser(int clientfd){
	NEWUSER user;
	user.fds=0;
	return Send(clientfd,(const char*)&user,sizeof(NEWUSER),0);
}

int processor(int fd){
	char buff[1024];
	int ret=Recv(fd,buff,sizeof(DATAHEADER),0);
	if(ret<=0) return -1;
	DATAHEADER *header=(DATAHEADER*)buff;
	switch (header->cmd) {
		case CMD_NEW_USER:{
			Recv(fd,buff+sizeof(DATAHEADER),sizeof(NEWUSER)-sizeof(DATAHEADER),0);
			NEWUSER *user=(NEWUSER*)buff;
			printf("新的客户端加入,当前连接数:%d\n",user->fds);
			return 1;
		}

		case CMD_LOGIN_RESULT:{
			Recv(fd,buff+sizeof(DATAHEADER),sizeof(LOGINRESULT)-sizeof(DATAHEADER),0);
			LOGIN *login=(LOGIN*)buff;
			printf("收到客户端消息长度:%d，消息类型:%d\n",login->dataLength,login->cmd);
			return 1;
		}

		case CMD_LOGOUT_RESULT:{
			Recv(fd,buff+sizeof(DATAHEADER),sizeof(LOGOUTRESULT)-sizeof(DATAHEADER),0);
			LOGOUTRESULT *ret=(LOGOUTRESULT*)buff;
			printf("收到客户端消息长度:%d，消息类型:%d\n",ret->dataLength,ret->cmd);
			return 1;
		}

	}
	return -1;
}

int main(){
	int connfd;
	struct sockaddr_in addr={};
	fd_set fdRead;
	timeval tv;
	char writebuff[1024]={0};
	char readbuff[1024]={0};

	connfd=Socket(AF_INET,SOCK_STREAM,0);

	addr.sin_family=AF_INET;
	addr.sin_port=htons(8000);
	inet_pton(AF_INET,"127.0.0.1",&addr.sin_addr);
	Connect(connfd,(const struct sockaddr*)&addr,sizeof(sockaddr_in));
	//直接向客户端发送新用户消息

	sendNewUser(connfd);

	while(true){
		
	}
	return 0;
}
