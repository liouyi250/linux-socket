#ifndef _CLIENT_H
#define _CLIENT_H

#include <iostream>
#include "common/socketwrap.h"

#ifndef BUFF_SIZE
#define BUFF_SIZE 10240
#endif 

class Client{
private:
  int connfd;
  char firstBuff[BUFF_SIZE*10];
  char secondBuff[BUFF_SIZE];
  int lastPos;
public:
  Client(){
    connfd=-1;
    lastPos=0;
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

  int send(DATAHEADER *header,int size){
      return Send(connfd,(const char*)header,size,0);
  }

  int recv(char *buff,int size){
      return Recv(connfd,buff,size,0);
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

//SOCKET默认的接收缓冲区有8K，如果数据发送的频率太快，会造成阻塞，需要一个缓冲区来接收默认缓存区中的数据
  int processor(){
    int ret=Recv(connfd,secondBuff,BUFF_SIZE,0);//全部从socket默认缓冲区里面接收完
    if(ret<=0) {
      this->close();
      return -1;
    }
    memcpy(firstBuff+lastPos,secondBuff,ret);//将字符串从第二缓冲区拷贝到第一缓冲区
    lastPos+=ret;
    while(lastPos>=sizeof(DATAHEADER)){//判断第一缓冲区内的长度是否有一个头部长度
      DATAHEADER *header=(DATAHEADER*)firstBuff;
      if(lastPos>=header->dataLength){//判断缓冲区内的长度是否是一个完整的消息
        int size=lastPos-header->dataLength;//保存长度
        onNetMsg(header);
        memcpy(firstBuff,firstBuff+header->dataLength,size);//将剩余未处理的字符串拷贝到缓冲区头部
        lastPos=size;//改变缓冲区内长度
      }else{
        break;
      }
    }
    return 0;
  }

  void onNetMsg(DATAHEADER *header){
    switch (header->cmd) {
  		case CMD_NEW_USER:{
  			NEWUSER *user=(NEWUSER*)header;
//  			printf("新的客户端加入,当前连接数:%d\n",user->fds);
  			break;
  		}

  		case CMD_LOGIN_RESULT:{
  			LOGIN *login=(LOGIN*)header;
  //			printf("收到服务端消息长度:%d，消息类型:%d\n",login->dataLength,login->cmd);
  			break;
  		}

  		case CMD_LOGOUT_RESULT:{
  			LOGOUTRESULT *ret=(LOGOUTRESULT*)header;
  //			printf("收到服务端消息长度:%d，消息类型:%d\n",ret->dataLength,ret->cmd);
  			break;
  		}

  		case CMD_EXIT:{
  			USEREXIT *ret=(USEREXIT*)header;
  //			printf("有客户端退出，当前连接数%d\n",ret->fds);
  			break;
  		}
      default:{
        printf("错误\n");
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
