#include "common/socketwrap.h"
#include <string.h>
#include <vector>
#include <stdio.h>
#include <algorithm>

#include "server.hpp"
//select(int nfds,fd_set *readfds,fd_set *writefds,fd_set *exceptfds,strcut timeval *timeout);
//nfds在fd_set最大描述符,在linux中就是listenfd+1
//readfds，传入可读的socket集合
//writefds，传入可写的socket集合
//exceptfds,传入异常的socket集合
//timeout,延迟，如果为空，则为阻塞模式，如果传入非空值，则超时后会返回，变为非阻塞模式
//select WINDOWS下默认最大可以处理64个socket



int main(){
	Server server;
	server.bind("127.0.0.1",8000);
	server.listen(30);

	while(true){
		server.onSelect();
	}

	return 0;
}
