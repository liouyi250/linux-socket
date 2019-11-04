#ifndef _CLIENT_H
#define _CLIENT_H

#include <iostream>
#include "common/socketwrap.h"



class Client{
private:
  int connfd;

public:
  Client(){
    connfd=-1;
  }

  ~Client(){
    this->close();
  #ifdef _WIN32
    WSACleanup();
  #endif
  }

  void init(){
    #ifdef _WIN32
  		WSADATA data;
  		int ret=WSAStartup(MAKEWORD(2,2),&data);
  		if(ret!=0){
  			printf("初始化套接字环境失败\n");
  			exit(0);
  		}
  	#endif
    connfd=Socket(AF_INET,SOCK_STREAM,0);
  }

  int connect(const char *ip,unsigned short port){
      if(connfd==-1) {
        init();
      }
      sockaddr_in addr;
      addr.sin_family=AF_INET;
    	addr.sin_port=htons(port);
    	#ifndef _WIN32
    		inet_pton(AF_INET,ip,&addr.sin_addr);
    	#else
    		addr.sin_addr.S_un.S_addr=inet_addr(ip);
    	#endif
      return Connect(connfd,(const struct sockaddr*)&addr,sizeof(sockaddr_in));
  }

  int send(DATAHEADER *header){
      return Send(connfd,(const char*)header,header->dataLength,0);
  }

  int recv(char *buff){
      return Recv(connfd,buff,sizeof(buff),0);
  }

  void onSelect(){
    if(!isRun()) return;

    fd_set fdRead;
    timeval tv;

    FD_ZERO(&fdRead);
		FD_SET(connfd,&fdRead);

		tv.tv_sec=0;
		tv.tv_usec=0;

		Select(connfd+1,&fdRead,NULL,NULL,&tv);

		if(FD_ISSET(connfd,&fdRead)){
			FD_CLR(connfd,&fdRead);
			if(-1==processor()){
				close();
			}
		}
  }


  int processor(){
  	char buff[1024];
  	int ret=Recv(connfd,buff,sizeof(DATAHEADER),0);
  	if(ret<=0){
      close();
      return -1;
    }
  	DATAHEADER *header=(DATAHEADER*)buff;
    Recv(connfd,buff+sizeof(DATAHEADER),header->dataLength-sizeof(DATAHEADER),0);
    onNetMsg(header);
  	return 0;
  }

  void onNetMsg(DATAHEADER *header){
    switch (header->cmd) {
  		case CMD_NEW_USER:{
  			NEWUSER *user=(NEWUSER*)header;
  			printf("新的客户端加入,当前连接数:%d\n",user->fds);
  			break;
  		}

  		case CMD_LOGIN_RESULT:{
  			LOGIN *login=(LOGIN*)header;
  			printf("收到服务端消息长度:%d，消息类型:%d\n",login->dataLength,login->cmd);
  			break;
  		}

  		case CMD_LOGOUT_RESULT:{
  			LOGOUTRESULT *ret=(LOGOUTRESULT*)header;
  			printf("收到服务端消息长度:%d，消息类型:%d\n",ret->dataLength,ret->cmd);
  			break;
  		}

  		case CMD_EXIT:{
  			USEREXIT *ret=(USEREXIT*)header;
  			printf("有客户端退出，当前连接数%d\n",ret->fds);
  			break;
  		}
    }
  }

  bool isRun(){
    return connfd!=-1;
  }

  int close(){
    return Close(connfd);
  }

};

#endif
