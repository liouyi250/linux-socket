linux下使用以下命令编译
g++ tcpserver-1.cpp common/socketwrap.cpp -o server -pthread -std=c++11
g++ tcpclient-1.cpp common/socketwrap.cpp -o client -pthread -std=c++11

windows下使用以下命令编译(使用dev-5.11编译器)
g++ -c common/socketwrap.cpp -lws2_32
g++ tcpserver-1.cpp common/socketwrap.o -o server -pthread -std=c++11 -lws2_32
g++ tcpclient-1.cpp common/socketwrap.o -o client -pthread -std=c++11 -lws2_32
