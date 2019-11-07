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
			client->send(&login,sizeof(LOGIN));
		}else if(strcmp(cmdBuff,"logout")==0){
			LOGOUT out;
			strcpy(out.username,"张三");
			client->send(&out,sizeof(LOGOUT));
		}else if(0==strcmp(cmdBuff,"exit")){
			g_bloops=false;
			break;
		}
	}
}

int main(){
	const int n=63;
	Client *client[n];
	for(int i=0;i<n;i++){
		client[i]=new Client();
		client[i]->init();
	}

	for(int i=0;i<n;i++){
		client[i]->connect("127.0.0.1",8000);
	}

	LOGIN login;
	strcpy(login.username,"张三");
	strcpy(login.password,"李四");
	int size=sizeof(LOGIN);
	while(g_bloops){
		for(int i=0;i<n;i++){
		client[i]->send(&login,size);
                client[i]->onSelect();

	}
		
//		client1.onSelect();

	}

	for(int i=0;i<n;i++){
		client[i]->close();

	}
	
	return 0;
}
