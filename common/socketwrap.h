#include <sys/types.h>
#include <sys/socket.h> //basic socket definitions
#include <netinet/in.h> //sockaddr struct header
#include <unistd.h> //read and write function header


//网络数据报文定义包含包头和包体
///包头描述消息包大小和作用，一般为int型
enum CMD{
	CMD_LOGIN,
	CMD_LOGIN_RESULT,
	CMD_LOGOUT,
	CMD_LOGOUT_RESULT,
	CMD_ERROR
};
//包头
struct DATAHEADER{
	int dataLength;//数据长度
	short cmd;//命令
};
//包体
struct LOGIN:public DATAHEADER{
	LOGIN(){
		dataLength=sizeof(LOGIN);
		cmd=CMD_LOGIN;
	}
	char username[32];
	char password[32];
};

struct LOGINRESULT:public DATAHEADER{
	LOGINRESULT(){
		dataLength=sizeof(LOGINRESULT);
		cmd=CMD_LOGIN_RESULT;
		result=0;
	}
	int result;

};

struct LOGOUT:public DATAHEADER{
	LOGOUT(){
		cmd=CMD_LOGOUT;
		dataLength=sizeof(LOGOUT);
	}
	char username[32];
};

struct LOGOUTRESULT:public DATAHEADER{
	LOGOUTRESULT(){
		cmd=CMD_LOGOUT_RESULT;
		dataLength=sizeof(LOGOUTRESULT);
		result=0;
	}	
	int result;
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


