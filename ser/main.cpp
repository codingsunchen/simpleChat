#include<iostream>
#include"tcpsever.h"
#include <stdlib.h>
using namespace std;

//   ./main ip port pthread_num
int main(int argc,char *argv[])
{
	if(argc < 4)
	{
		cout<<"参数个数不够"<<endl;
		return 0;
	}
	//分离参数
	char *ip = argv[1];
	short port = atoi(argv[2]);
	int pthread_num = atoi(argv[3]);
	Tcpserver server(ip,port,pthread_num);//用Tcpserver类构造一个sever对象。把服务器封装为了一个类。
	server.run();//调用run函数让服务器跑起来。
}


