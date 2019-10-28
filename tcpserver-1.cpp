#include "common/socketwrap.h"
#include <string.h>
#include <vector>
#include <stdio.h>
#include <algorithm>
//select(int nfds,fd_set *readfds,fd_set *writefds,fd_set *exceptfds,strcut timeval *timeout);
//nfds在fd_set最大描述符,在linux中就是listenfd+1
//readfds，传入可读的socket集合
//writefds，传入可写的socket集合
//exceptfds,传入异常的socket集合
//timeout,延迟，如果为空，则为阻塞模式，如果传入非空值，则超时后会返回，变为非阻塞模式
//select 最大可以处理64个socket

int processor(int fd){
	char buff[1024];
	int ret=Recv(fd,buff,sizeof(DATAHEADER),0);
	if(ret<=0) return -1;
	DATAHEADER *header=(DATAHEADER*)buff;
	switch (header->cmd) {
		case CMD_NEW_USER:{
			Recv(fd,buff+sizeof(DATAHEADER),sizeof(NEWUSER)-sizeof(DATAHEADER),0);
			NEWUSER *user=(NEWUSER*)buff;
			return 1;
		}
		break;
		case CMD_LOGIN:{
			Recv(fd,buff+sizeof(DATAHEADER),sizeof(LOGIN)-sizeof(DATAHEADER),0);
			LOGIN *user=(LOGIN*)buff;
			printf("收到客户端消息长度:%d，消息类型:%d,username=%s,password=%s\n",user->dataLength,user->cmd,user->username,user->password);
			LOGINRESULT ret;
			return Send(fd,(const char*)&ret,sizeof(LOGINRESULT),0);
		}
		break;
		case CMD_LOGOUT:{
			Recv(fd,buff+sizeof(DATAHEADER),sizeof(LOGOUT)-sizeof(DATAHEADER),0);
			LOGOUT *user=(LOGOUT*)buff;
			printf("收到客户端消息长度:%d，消息类型:%d,username=%s\n",user->dataLength,user->cmd,user->username);
			LOGOUTRESULT ret;
			return Send(fd,(const char*)&ret,sizeof(LOGOUTRESULT),0);
		}
		break;
		case CMD_EXIT:{

		}
		return -1;
	}
	return -1;
}

int main(){
	int listenfd,connfd,maxsock;
	struct sockaddr_in servaddr,clientAddr;
	std::vector<int> g_clients;
	struct timeval timeout;
	char buff[128];
#ifdef _WIN32
	WSADATA data;
		int ret=WSAStartup(MAKEWORD(2,2),&data);
		if(ret!=0){
			printf("初始化套接字环境失败\n");
			return 0;
		}
#endif
	listenfd=Socket(AF_INET,SOCK_STREAM,0);
//	bzero(&servaddr,sizeof(servaddr));
	servaddr.sin_family=AF_INET;
	servaddr.sin_addr.s_addr=htonl(INADDR_ANY);
	servaddr.sin_port=htons(8000);

	Bind(listenfd,(const struct sockaddr*)&servaddr,sizeof(servaddr));
	Listen(listenfd,30);

	//可读的集合
	fd_set fdRead;
	maxsock=listenfd;

	for(;;){
		buff[0]='\0';
		//立即返回
		timeout.tv_sec=30;
		timeout.tv_usec=0;
		//清空集合
		FD_ZERO(&fdRead);
		//将fd_set添加到集合中
		FD_SET(listenfd,&fdRead);

		for(int i=0;i<g_clients.size();i++){
			FD_SET(g_clients[i],&fdRead); //由于之前将fdRead清零了，这里需要把所有的套接字加入到fdRead
		}

		Select(maxsock+1,&fdRead,NULL,NULL,&timeout);

		for(int i=0;i<g_clients.size();i++){
			if(FD_ISSET(g_clients[i],&fdRead)){//在vector中的套接字可读
				FD_CLR(g_clients[i],&fdRead);  //可读的集合中收到数据，将该socket移出集合中
				if(-1==processor(g_clients[i])){//客户端退出
					printf("客户端%d退出回话\n",g_clients[i]);
					Close(g_clients[i]);
					std::vector<int>::iterator iter=std::find(g_clients.begin(),g_clients.end(),g_clients[i]);
					g_clients.erase(iter);
					//广播消息
					for(int i=0;i<g_clients.size();i++){//有客户端退出，广播消息
							USEREXIT user;
							user.fds=g_clients.size();//客户端总数
						  Send(g_clients[i],(const char*)&user,sizeof(USEREXIT),0);
					}
				}
			}
		}

		if(FD_ISSET(listenfd,&fdRead)){
			socklen_t socklen=sizeof(sockaddr_in);
			connfd=Accept(listenfd,(sockaddr*)&clientAddr,&socklen);
			g_clients.push_back(connfd);//保存套接字描述符到vector中
			for(int i=0;i<g_clients.size();i++){//向所有的客户端广播消息，有新的客户加入
					NEWUSER user;
					user.fds=g_clients.size();//客户端总数
				  Send(g_clients[i],(const char*)&user,sizeof(NEWUSER),0);
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
	Close(listenfd);
	#ifdef _WIN32
	WSACleanup(); 
	#endif
	return 0;
}
