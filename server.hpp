#ifndef _SERVER_HPP
#define _SERVER_HPP

#include <vector>
#include <string.h>
#include "common/socketwrap.h"

#ifndef BUFF_SIZE
#define BUFF_SIZE 10240
#endif 

//由于服务端和多个客户端进行链接，所以需要识别每一个客户端，客户端的数据存储在自己空间中，这样不会造成数据混淆
class ClientSocket{
  private:
    int client;
    char firstBuff[BUFF_SIZE*10];//客户端的数据缓冲区
    int lastPos;
  public:
  ClientSocket(int sock=-1){
    client=sock;
    memset(firstBuff,0,BUFF_SIZE*10);
    lastPos=0;
  }

  char *msgBuf(){
    return firstBuff;
  }

  int getLastPos(){
    return lastPos;
 }

  void setLastPos(int pos){
    lastPos=pos;
  }

  int getfd() const{
    return client;
  }

};


class Server{
private:
  int listenfd;
  std::vector<ClientSocket*> clients;
  int maxsock;
  char secondBuff[BUFF_SIZE];//第二缓冲区
public:
  Server(){
    listenfd=-1;
  }

  ~Server(){
    for(int n=0;n<clients.size();n++){
      this->close(clients[n]);
    }
    this->close(listenfd);

  #ifdef _WIN32
    WSACleanup();
  #endif

  }
  //初始化网络环境
  void init(){
    #ifdef _WIN32
    	WSADATA data;
    	int ret=WSAStartup(MAKEWORD(2,2),&data);
    	if(ret!=0){
      	printf("初始化套接字环境失败\n");
      	return;
    	}
    #endif
    	listenfd=Socket(AF_INET,SOCK_STREAM,0);
      maxsock=listenfd;
  }

  int bind(const char *ip,unsigned short port){
    if(listenfd==-1){
      init();
    }
    sockaddr_in servaddr;
    servaddr.sin_family=AF_INET;
  #ifdef _WIN32
    if(ip)
      servaddr.sin_addr.S_un.S_addr=inet_addr(ip);
    else
  	 servaddr.sin_addr.S_un.S_addr=htonl(INADDR_ANY);
  #else
    if (ip)
      inet_pton(AF_INET,ip,&servaddr.sin_addr);
    else
      servaddr.sin_addr.s_addr=htonl(INADDR_ANY);
  #endif
  	servaddr.sin_port=htons(port);
    return Bind(listenfd,(const struct sockaddr*)&servaddr,sizeof(servaddr));
  }

  int listen(int backlog){
    return Listen(listenfd,backlog);
  }

  void close(ClientSocket *client){
    Close(client->getfd());
    delete client;
    client=NULL;
  }

  void close(int fd){
    Close(fd);
  }

  void onSelect(){
    //立即返回
		timeval tv={0,0};
    fd_set fdRead;
		//清空集合
		FD_ZERO(&fdRead);
		//将fd_set添加到集合中
		FD_SET(listenfd,&fdRead);

		for(int i=0;i<clients.size();i++){
			FD_SET(clients[i]->getfd(),&fdRead); //由于之前将fdRead清零了，这里需要把所有的套接字加入到fdRead
		}

		Select(maxsock+1,&fdRead,NULL,NULL,&tv);

    for(int i=0;i<clients.size();i++){
      if(FD_ISSET(clients[i]->getfd(),&fdRead)){
        FD_CLR(clients[i]->getfd(),&fdRead);
        if(-1==processor(clients[i])){
          std::vector<ClientSocket*>::iterator iter=clients.begin()+i;
          if(iter!=clients.end()){
            printf("<%d>客户端退出\n",clients[i]->getfd());
            delete clients[i];
            clients[i]=nullptr;
            clients.erase(iter);
          }
        }
      }
    }
    	//广播消息
          /*
					for(int i=0;i<clients.size();i++){//有客户端退出，广播消息
							USEREXIT user;
							user.fds=clients.size();//客户端总数
						  send(clients[i],&user);
					}
          */


		if(FD_ISSET(listenfd,&fdRead)){
                        FD_CLR(listenfd,&fdRead);
                        sockaddr_in clientAddr;
			socklen_t socklen=sizeof(sockaddr_in);
			int connfd=Accept(listenfd,(sockaddr*)&clientAddr,&socklen);
                        ClientSocket *client=new ClientSocket(connfd);
			clients.push_back(client);//保存套接字描述符到vector中
      /*
			for(int i=0;i<clients.size();i++){//向所有的客户端广播消息，有新的客户加入
					NEWUSER user;
					user.fds=clients.size();//客户端总数
				  send(clients[i],&user);
			}
      */
			char IPaddress[32];
			#ifndef _WIN32
				inet_ntop(AF_INET,&clientAddr.sin_addr,IPaddress,32);
			#else
				strcpy(IPaddress,inet_ntoa(clientAddr.sin_addr));
			#endif
			printf("新客户端加入：socket=%d,IP=%s\n",connfd,IPaddress);
			if(connfd>maxsock){
				maxsock=connfd; //每次有个新的客户端链接都要保证maxsock是最大的
			}
		}
  }

  void onNetMsg(int fd,DATAHEADER *header){
    switch (header->cmd) {
      case CMD_NEW_USER:{
        NEWUSER *user=(NEWUSER*)header;
      }
      break;
      case CMD_LOGIN:{
        LOGIN *user=(LOGIN*)header;
  //      printf("收到客户端消息长度:%d，消息类型:%d,username=%s,password=%s\n",user->dataLength,user->cmd,user->username,user->password);
        LOGINRESULT ret;
        send(fd,&ret);
      }
      break;
      case CMD_LOGOUT:{
        LOGOUT *user=(LOGOUT*)header;
  //      printf("收到客户端消息长度:%d，消息类型:%d,username=%s\n",user->dataLength,user->cmd,user->username);
  //      LOGOUTRESULT ret;
   //     send(fd,&ret);
      }
      break;
      
      case CMD_EXIT:{

      }
      break;
      default:{
        printf("错误\n");
      }
      break;
  }
}

int processor(ClientSocket *client){
    int ret=Recv(client->getfd(),secondBuff,BUFF_SIZE,0);//全部从socket默认缓冲区里面接收完
    if(ret<=0) {
      return -1;
    }
    memcpy(client->msgBuf()+client->getLastPos(),secondBuff,ret);//将字符串从第二缓冲区拷贝到第一缓冲区
    client->setLastPos(client->getLastPos()+ret);
    while(client->getLastPos()>=sizeof(DATAHEADER)){//判断第一缓冲区内的长度是否有一个头部长度
      DATAHEADER *header=(DATAHEADER*)client->msgBuf();
      if(client->getLastPos()>=header->dataLength){//判断缓冲区内的长度是否是一个完整的消息
        int size=client->getLastPos()-header->dataLength;//保存长度
        onNetMsg(client->getfd(),header);
        memcpy(client->msgBuf(),client->msgBuf()+header->dataLength,size);//将剩余未处理的字符串拷贝到缓冲区头部
        client->setLastPos(size);//改变缓冲区内占用长度
      }else{
        break;//一定要break，否则数据长度不够会死循环
      }
    }
    return 0;
  }

  int send(int connfd,DATAHEADER *header){
    return Send(connfd,(const char*)header,header->dataLength,0);
  }

  int recv(int connfd,char *buff){
    return Recv(connfd,buff,sizeof(buff),0);
  }

};


#endif
