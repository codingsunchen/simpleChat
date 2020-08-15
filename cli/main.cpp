#include<iostream>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <arpa/inet.h>
#include <json/json.h>
#include <sys/socket.h>
#include <assert.h>
#include <signal.h>
#include <errno.h>
#include <sys/types.h>
#include <assert.h>
#include "fun.h"
#include "public.h"
#include <stdio.h>
#include <unistd.h>
using namespace std;

int clifd = -1;

void sig_fun(int)
{
	do_exit(clifd);
	signal(SIGINT,SIG_DFL);
	exit(0);
}

int main(int argc,char *argv[])
{
	//绑定信号处理函数（do_exit）;
	signal(SIGINT,sig_fun);
	/*
	if(argc < 3)
	{
		cerr<<"cmd is not enough;errno:"<<errno<<endl;
		return 0;
	}

	//分离ip地址和端口号
	//char *ip = argv[1];
	char ip[17] = {0};
	strcpy(ip,argv[1]);
	short port = atoi(argv[2]);
	*/
	//链接服务qi
    //clifd = socket(AF_INET,SOCK_STREAM,0);
	struct sockaddr_in saddr;
	memset(&saddr,0,sizeof(saddr));
	saddr.sin_family = AF_INET;
	saddr.sin_port = htons(6000);
	saddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	int connect_num = 0;
	int res;
	while(1)
	{
    clifd = socket(AF_INET,SOCK_STREAM,0);
		res = connect(clifd,(struct sockaddr*)&saddr,sizeof(saddr));
		
			if(-1 == res)
			{
				perror("connect error:");
				//return 0;
				break;
			}
			connect_num++;
		
	cout<<connect_num<<endl;
	}

	while(1)
	{
		//让用户选择服务
		cout<<"选择服务：	1.register  2.login   3.exit"<<endl;
		int select;
		cin>>select;
		//getchar();
		switch(select)
		{
			case 1:
				do_register(clifd);break;
			case 2:
				do_login(clifd);break;
			case 3:
				do_exit(clifd);return 0;break;
			default:
				cout<<"输入有错，请重新输入选择服务:"<<endl;break;
		}
	}
	
	return 0;
}
