#include "./common/socketwrap.h"
#include <string.h>
#include <cstdio>
#include <arpa/inet.h>
int main(){
	int connfd;
	struct sockaddr_in addr={};
	char writebuff[1024]={0};
	char readbuff[1024]={0};

	connfd=Socket(AF_INET,SOCK_STREAM,0);

	addr.sin_family=AF_INET;
	addr.sin_port=htons(8000);
	inet_pton(AF_INET,"127.0.0.1",&addr.sin_addr);
	Connect(connfd,(const struct sockaddr*)&addr,sizeof(sockaddr_in));
	
	while(true){
		DATAHEADER header;
		char cmdBuff[128];
		scanf("%s",&cmdBuff);
		if(strcmp(cmdBuff,"login")==0){
			LOGIN login;
			printf("%d,%d\n",login.cmd,login.dataLength);
			strcpy(login.username,"张三");
			strcpy(login.password,"1234");
			printf("%s,%s\n",login.username,login.password);
			Write(connfd,(const char*)&login,sizeof(LOGIN)); //!!!发送数据使用const char*，接收数据使用char*,不然数据会有问题
			LOGINRESULT ret;
			Read(connfd,(char*)&ret,sizeof(LOGINRESULT));
			printf("收到命令:%d,命令长度:%d\n",ret.cmd,ret.dataLength);
		}else if(strcmp(cmdBuff,"logout")==0){
			LOGOUT out;
			strcpy(out.username,"张三");
			Write(connfd,(const char*)&out,sizeof(LOGOUT));
			LOGOUTRESULT outRet;
			Read(connfd,(char*)&outRet,sizeof(LOGOUTRESULT));
			printf("收到命令:%d,命令长度:%d\n",outRet.cmd,outRet.dataLength);
		}else{
		}
		

	}
	
	Close(connfd);
	return 0;
}
