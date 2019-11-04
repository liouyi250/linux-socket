#include <thread>
#include <string.h>
#include <stdio.h>
#include "client.hpp"
bool g_bloops=true;

void sendCmd(Client *client){//线程处理函数
	while(1){
		char cmdBuff[128];
		scanf("%s",&cmdBuff);
		if(strcmp(cmdBuff,"login")==0){
			LOGIN login;
			strcpy(login.username,"张三");
			strcpy(login.password,"1234");
			client->send(&login);
		}else if(strcmp(cmdBuff,"logout")==0){
			LOGOUT out;
			strcpy(out.username,"张三");
			client->send(&out);
		}else if(0==strcmp(cmdBuff,"exit")){
			g_bloops=false;
			break;
		}
	}
}

int main(){
	Client client1;
	client1.init();
	client1.connect("127.0.0.1",8000);
	//直接向客户端发送新用户消息
	NEWUSER user1;
	client1.send(&user1);
	//启动发送消息线程
	std::thread t1(sendCmd,&client1);
	t1.detach();




	while(g_bloops){
		client1.onSelect();

	}
	client1.close();

	return 0;
}
