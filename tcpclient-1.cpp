#include "./common/socketwrap.h"
#include <string.h>
#include <cstdio>
#include <arpa/inet.h>
#include <thread>

bool g_bloops=true;

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
			printf("收到服务端消息长度:%d，消息类型:%d\n",login->dataLength,login->cmd);
			return 1;
		}

		case CMD_LOGOUT_RESULT:{
			Recv(fd,buff+sizeof(DATAHEADER),sizeof(LOGOUTRESULT)-sizeof(DATAHEADER),0);
			LOGOUTRESULT *ret=(LOGOUTRESULT*)buff;
			printf("收到服务端消息长度:%d，消息类型:%d\n",ret->dataLength,ret->cmd);
			return 1;
		}

		case CMD_EXIT:{
			Recv(fd,buff+sizeof(DATAHEADER),sizeof(USEREXIT)-sizeof(DATAHEADER),0);
			USEREXIT *ret=(USEREXIT*)buff;
			printf("有客户端退出，当前连接数%d\n",ret->fds);
			return 1;
		}
	}
	return -1;
}

void sendCmd(int fd){//线程处理函数
	while(1){
		char cmdBuff[128];
		scanf("%s",&cmdBuff);
		if(strcmp(cmdBuff,"login")==0){
			LOGIN login;
			strcpy(login.username,"张三");
			strcpy(login.password,"1234");
			Send(fd,(const char*)&login,sizeof(LOGIN),0);
		}else if(strcmp(cmdBuff,"logout")==0){
			LOGOUT out;
			strcpy(out.username,"张三");
			Send(fd,(const char*)&out,sizeof(LOGOUT),0);
		}else if(0==strcmp(cmdBuff,"exit")){
			g_bloops=false;
		}
	}
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
	//启动发送消息线程
	std::thread t1(sendCmd,connfd);
	t1.detach();

	while(g_bloops){
		FD_ZERO(&fdRead);
		FD_SET(connfd,&fdRead);

		tv.tv_sec=0;
		tv.tv_usec=0;

		Select(connfd+1,&fdRead,NULL,NULL,&tv);

		if(FD_ISSET(connfd,&fdRead)){
			FD_CLR(connfd,&fdRead);
			if(-1==processor(connfd)){
				Close(connfd);
			}
		}
	}
	return 0;
}
