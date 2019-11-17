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

const int n=1000;
const int t=4;
Client *client[n];

void sendThread(int id){
	int start=n/t*(id-1);
	int end=n/t*id;
	for(int i=start;i<end;i++){
		client[i]=new Client();
	}

	for(int i=start;i<end;i++){
		client[i]->connect("127.0.0.1",8000);
	}

	LOGIN login;
	strcpy(login.username,"张三");
	strcpy(login.password,"李四");
	int size=sizeof(LOGIN);
	while(g_bloops){
		for(int i=start;i<end;i++){
			client[i]->send(&login,size);
		}
	}

	for(int i=start;i<end;i++){
		client[i]->close();
		delete client[i];
		client[i]=NULL;
	}
}

int main(){

	for(int i=0;i<t;i++){
		std::thread th(sendThread,i+1);
		th.detach();
	}
	while(1){
		sleep(1);
	}
	

	return 0;
}
