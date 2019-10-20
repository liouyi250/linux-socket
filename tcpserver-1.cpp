#include "./common/socketwrap.h"
#include <string.h>
#include <cstdio>
int main(){
	int listenfd,connfd;
	struct sockaddr_in servaddr;
	char buff[1024];

	listenfd=Socket(AF_INET,SOCK_STREAM,0);
	bzero(&servaddr,sizeof(servaddr));
	servaddr.sin_family=AF_INET;
	servaddr.sin_addr.s_addr=htonl(INADDR_ANY);
	servaddr.sin_port=htons(8000);

	Bind(listenfd,(const struct sockaddr*)&servaddr,sizeof(servaddr));

	Listen(listenfd,5);

	for(;;){
		connfd=Accept(listenfd,(sockaddr*)NULL,NULL);
		DATAHEADER header;
		Read(connfd,&header,sizeof(DATAHEADER));
		printf("数据长度=%d,消息命令=%d\n",header.dataLength,header.cmd);
		switch(header.cmd){
			case CMD_LOGIN:{
					LOGIN login;
					LOGINRESULT ret;
					Read(connfd,(char*)&login+sizeof(DATAHEADER),sizeof(LOGIN)-sizeof(DATAHEADER));  //!!!发送数据使用const char*，接收数据使用char*,不然数据会有问题
					printf("username=%s,password=%s\n",login.username,login.password);
					Write(connfd,(const char*)&ret,sizeof(LOGINRESULT));
				}
				break;
			case CMD_LOGOUT:{
					LOGOUT out;
					LOGOUTRESULT outRet;
					Read(connfd,(char*)&out+sizeof(DATAHEADER),sizeof(LOGOUT)-sizeof(DATAHEADER));
					Write(connfd,(const char*)&outRet,sizeof(LOGOUTRESULT));
				}
				break;
			default:
				Write(connfd,"不支持的命令",7);
				break;
		}

		Close(connfd);
	}
	return 0;
	

}
