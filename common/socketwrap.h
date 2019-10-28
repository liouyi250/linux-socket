#ifndef _SOCKETWRAP_H
#define _SOCKETWRAP_H
#define WIN32_LEAN_AND_MEAN
#ifdef _WIN32
	#include <winsock2.h>
	#include <windows.h>
	#define socklen_t int
#else
	#include <sys/types.h>
	#include <sys/socket.h> //basic socket definitions
	#include <netinet/in.h> //sockaddr struct header
	#include <unistd.h> //read and write function header
	#include <arpa/inet.h>
#endif



//网络数据报文定义包含包头和包体
//包头描述消息包大小和作用，一般为int型
enum CMD{
	CMD_LOGIN,
	CMD_LOGIN_RESULT,
	CMD_LOGOUT,
	CMD_LOGOUT_RESULT,
	CMD_NEW_USER,
	CMD_EXIT,
	CMD_ERROR
};
//包头
struct DATAHEADER{
	int dataLength;//数据长度
	short cmd;//命令
};
//包体
struct LOGIN:public DATAHEADER{//客户端登录
	LOGIN(){
		dataLength=sizeof(LOGIN);
		cmd=CMD_LOGIN;
	}
	char username[32];
	char password[32];
};

struct LOGINRESULT:public DATAHEADER{//登录结果
	LOGINRESULT(){
		dataLength=sizeof(LOGINRESULT);
		cmd=CMD_LOGIN_RESULT;
		result=0;
	}
	int result;

};

struct LOGOUT:public DATAHEADER{//客户端登出
	LOGOUT(){
		cmd=CMD_LOGOUT;
		dataLength=sizeof(LOGOUT);
	}
	char username[32];
};

struct LOGOUTRESULT:public DATAHEADER{//登出结果
	LOGOUTRESULT(){
		cmd=CMD_LOGOUT_RESULT;
		dataLength=sizeof(LOGOUTRESULT);
		result=0;
	}
	int result;
};

struct NEWUSER:public DATAHEADER{//新的客户端加入
	NEWUSER(){
		cmd=CMD_NEW_USER;
		dataLength=sizeof(NEWUSER);
	}
	int fds;//当前连接数
};

struct USEREXIT:public DATAHEADER{//客户端退出
	USEREXIT(){
		cmd=CMD_EXIT;
		dataLength=sizeof(USEREXIT);
	}
	int fds;//当前连接数
};

//进阶,使用继承的方法
/*
struct LOGIN:public DATAHEADER{
	char username[32];
	char password[32];

	LOGIN(){
		cmd=CMD_LOGIN;
		dataLength=sizeof(LOGIN);
	}
};
*/

int Socket(int domain,int type,int protocol);

int Listen(int sockfd,int backlog);

int Bind(int sockfd,const struct sockaddr* addr,socklen_t addrlen);

int Accept(int sockfd,struct sockaddr *addr,socklen_t *addrlen);

int Close(int fd);

int Connect(int sockfd,const struct sockaddr *addr,socklen_t addrlen);

int Read(int fd,void *buf,size_t count);

int Write(int fd,const void *buf,size_t count);

int Recv(int sockfd,char *buf,size_t len,int flags);

int Send(int sockfd,const char *buf,size_t len,int flags);

int Select(int nfds,fd_set *readfds,fd_set *writefds,fd_set *exceptfds,struct timeval *timeout);


#endif
