#ifndef _SERVER_HPP
#define _SERVER_HPP

#include <vector>
#include "common/socketwrap.h"

class Server{
private:
  int listenfd;
  std::vector<int> clients;
  int maxsock;
public:
  Server(){
    listenfd=-1;
  }

  ~Server(){
    this->close();
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

  void close(){
      for(int i=0;i<clients.size();i++){
        Close(clients[i]);
      }
      Close(listenfd);

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
			FD_SET(clients[i],&fdRead); //由于之前将fdRead清零了，这里需要把所有的套接字加入到fdRead
		}

		Select(maxsock+1,&fdRead,NULL,NULL,&tv);

		for(int i=0;i<clients.size();i++){
			if(FD_ISSET(clients[i],&fdRead)){//在vector中的套接字可读
				FD_CLR(clients[i],&fdRead);  //可读的集合中收到数据，将该socket移出集合中
				if(-1==processor(clients[i])){//客户端退出
					printf("客户端%d退出回话\n",clients[i]);
					Close(clients[i]);
					std::vector<int>::iterator iter=std::find(clients.begin(),clients.end(),clients[i]);
					clients.erase(iter);
					//广播消息
					for(int i=0;i<clients.size();i++){//有客户端退出，广播消息
							USEREXIT user;
							user.fds=clients.size();//客户端总数
						  send(clients[i],&user);
					}
				}
			}
		}

		if(FD_ISSET(listenfd,&fdRead)){
      sockaddr_in clientAddr;
			socklen_t socklen=sizeof(sockaddr_in);
			int connfd=Accept(listenfd,(sockaddr*)&clientAddr,&socklen);
			clients.push_back(connfd);//保存套接字描述符到vector中
			for(int i=0;i<clients.size();i++){//向所有的客户端广播消息，有新的客户加入
					NEWUSER user;
					user.fds=clients.size();//客户端总数
				  send(clients[i],&user);
			}
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
        printf("收到客户端消息长度:%d，消息类型:%d,username=%s,password=%s\n",user->dataLength,user->cmd,user->username,user->password);
        LOGINRESULT ret;
        send(fd,&ret);
      }
      break;
      case CMD_LOGOUT:{
        LOGOUT *user=(LOGOUT*)header;
        printf("收到客户端消息长度:%d，消息类型:%d,username=%s\n",user->dataLength,user->cmd,user->username);
        LOGOUTRESULT ret;
        send(fd,&ret);
      }
      break;
      case CMD_EXIT:{

      }
  }
}

  int processor(int fd){
    char buff[1024];
    int ret=Recv(fd,buff,sizeof(DATAHEADER),0);
    if(ret<=0) return -1;
    DATAHEADER *header=(DATAHEADER*)buff;
    Recv(fd,buff+sizeof(DATAHEADER),header->dataLength-sizeof(DATAHEADER),0);
    onNetMsg(fd,header);
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
