#include<iostream>
#include <stdlib.h>
#include <vector>
#include <map>
#include <functional>
#include <algorithm>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include "tcpsever.h"
#include <string.h>
#include <assert.h>
#include <stdio.h>
#include <event.h>
#include <json/json.h>
#include "public.h"

//该函数只能接受三个参数，所以不能定义为类的成员函数，要定义为类的友元函数。（若定义为类的友元函数，则会多一个this指针）
void listen_cb(int socketfd,short event,void *arg)
{
	Tcpserver *This = (Tcpserver*)arg;
	//接受用户链接
	struct sockaddr_in caddr;
	int len = sizeof(caddr);
	int cli_fd = accept(This->_listen_fd,(struct sockaddr*)&caddr,(socklen_t*)&len);//得到与客户端通信的cli_fd

	//查找当前监听数量最少的子线程
	//ser->_pth_work_num_map.sort();  error   map中没有sort函数
	
	map<int,int>::iterator it = (This->_pth_work_num_map).begin();
	int min = it->second;
	map<int,int>::iterator itmin = it;
	for(;it!=(This->_pth_work_num_map).end();++it)
	{
		if(it->second < min)
		{
			min = it->second;
			itmin = it;
		}
	}
	int sockpair_fd = itmin->first;

	//将客户端套接子通过socktpair发给子线程,让子线程中的libevent将该客户端套接子fd监听起来。
	//char *p = itoa(cli_fd);
	char p[8] = {0};
	sprintf(p,"%d",cli_fd);

	send(sockpair_fd,p,strlen(p),0);
}

void sock_pair_cb(int fd,short event,void *arg)
{
	cout<<"sock_pair_cb()"<<endl;
	Tcpserver *This = (Tcpserver*)arg;
	//读取管道内容
	char buff[16] = {0};
	recv(fd,buff,15,0);
	int num = atoi(buff);
	//更新到map表_pth_work_num  ----->fd
	This->_pth_work_num_map[fd] = num;
}

//构造函数完成初始化
Tcpserver::Tcpserver(char *ip,short port,int pth_num)
{
	//创建服务器
	_listen_fd = socket(AF_INET,SOCK_STREAM,0);
	assert(_listen_fd!=-1);
	struct sockaddr_in saddr;
	saddr.sin_family = AF_INET;
	saddr.sin_port = htons(port);
	saddr.sin_addr.s_addr = inet_addr(ip);
	int res = bind(_listen_fd,(struct sockaddr*)&saddr,sizeof(saddr));
	if(res == -1)
	{
		perror("bind error:");
		return;
	}
	listen(_listen_fd,5);

	_pth_num = pth_num;

	//给libevent申请空间
	_base = event_base_new();//主线程的libevent


	//创建事件，绑定监听套接子的回调函数(listen_cb)
	struct event* listen_event = event_new(_base,_listen_fd,EV_READ|EV_PERSIST,listen_cb,this);
	event_add(listen_event,NULL);//将listen_fd加入到主线程的libevent中监听起来。若有时间发生就去调用回调函数。
}

/*
Tcpserver::~Tcpserver()
{
	event_base_free(_base);
	vector<Pthread*>::iterator it = _pthread.begin();
	for(;it!=_pthread.end();++it)
	{
		delete *it;
	}
}
*/

void Tcpserver::run()
{
	//申请socketpair（函数自查）
	this->get_socket_pair();

	//创建线程
	this->get_pthread();

	//规定  int arr[2]  arr[0]<=>主线程占用   arr[1]<=>子线程占用


	//为主线程的socktpair创建事件，绑定回调函数（sock_pair_cb）	
	


	event_base_dispatch(this->_base);
	//event_free(listen_event);
}

void Tcpserver::get_socket_pair()
{
	for(int i = 0;i < _pth_num;i++ )
	{
		//申请双向管道
		int fd[2]={0};
		if(-1 == socketpair(AF_UNIX,SOCK_STREAM,0,fd))
		{
			perror("socketpair error:");
			return ;
		}
		//将双向管道加入到_sock_pair_vec.push_back();   要提供一个数组能够直接给数组进行赋值的函数重载。
		_socket_pair_vec.push_back(fd);
		struct event* listen_event = event_new(_base,fd[0],EV_READ|EV_PERSIST,listen_cb,this);
		event_add(listen_event,NULL);//将所有sockpair的0端加入到libevent中监听起来。

		//_pth_work_num_map.push_buck(makepair(fd[0],0));  在服务器的主线程中维护一张map表，专门用于记录各个子进程所监听的客户端链接的个数，用于在主线程中判断
		//子线程的压力。    规定socketpair中数组的0端让主线程拥有，1端让子线程拥有。
		_pth_work_num_map.insert(make_pair(fd[0],0));
	}


}

void Tcpserver::get_pthread()
{
	//开辟线程,并选择一个socketpair的1端交给子线程，让子线程用这个socketpair_fd与主线程之间进行通信。
	for(int i = 0; i< _pth_num; i++)
	{
		_pthread.push_back(new Pthread(_socket_pair_vec[i][1]));
	}
}

//////问题如何解决析构掉listen_event*事件的内存??????//////////////////////////




#if 0
////////////////////子线程函数/////////////////

void socketpair_cb(int fd,short event,void *arg);
void cli_cb(int fd,short event,void *arg);

Pthread::Pthread(int socketpairfd):socketpair_fd(socketpairfd),listen_cli_num(0)
{
	_base = event_base_new();
	//创建事件，绑定socketpair_fd的回调函数(socketpair_cb)
	struct event* listen_event = event_new(_base,socketpair_fd,EV_READ|EV_PERSIST,socketpair_cb,this);
	assert(listen_event != NULL);
	event_add(listen_event,NULL);//将listen_fd加入到主线程的libevent中监听起来。若有时间发生就去调用回调函数。
}

//对客户端请求的处理。      将客户端发过来的请求Json进行解析，得到消息类型，对不同类型的消息进行不同的处理方式。
void cli_cb(int clifd,short event,void *arg)//arg应该对应Pthread对象的指针this
{
	Pthread *This = (Pthread*)arg;
	char buff[128] = {0};
	if(-1 == recv(clifd,buff,127,0))
	{
		perror("cli_cb recv fail:");
		return ;
	}
	This->listen_cli_num++;//监听到新的客户端，将该子进程中的监听客户端总数目加1。
	Json::Value root;
	Json::Reader read;
	if(-1 == read.parse(buff,root))
	{
		perror("cli_cb json prase fail:");
		return ;
	}
	Msg_type select = (Msg_type)root["reason"].asInt();
	//对数据进行处理时，若客户端关闭了通信，要记得将listen_cli_num--;
	switch(select)
	{
		case REASON_TYPE_REGISTER:   /* 处理函数 */
			char buf[] = "error";
			send(clifd,buf,strlen(buf),0);
			break;
		case REASON_TYPE_LOGIN:/* 处理函数 */;break;
		case REASON_TYPE_LIST:/* 处理函数 */;break;
		case REASON_TYPE_TALK:/* 处理函数 */;break;
		case REASON_TYPE_GROUP:/* 处理函数 */;break;
		case REASON_TYPE_EXIT:/* 处理函数 */;break;
	}


}


//socketpair专门用于将主线程交给的客户端套接子监听起来，事件发生时调用回调函数进行处理。
void socketpair_cb(int fd,short event,void *arg)//arg应该接收的是子线程的对象this指针
{
	//struct event_base *_base = (struct event_base*)arg;
	Pthread *This = (Pthread*)arg;
	//接受主线程传送过来的客户端套接字
	char buff[16] = {0};
	if(-1 == recv(This->socketpair_fd,buff,15,0))
	{
		perror("recv error:");
		return ;
	}
	int clifd = atoi(buff);//得到主线程传给子线程的客户端描述符。
	//将客户端描述符构造为事件，交给子线程的libevent监听起来
	struct event* listen_event = event_new(This->_base,clifd,EV_READ|EV_PERSIST,cli_cb,This);//此处传给回调函数的参数应该是将事件的指针传过去，最终要在回调函数中将该
	                        															//在堆上的内存删除掉。需要修改
	assert(listen_event != NULL);
	event_add(listen_event,NULL);
}
#endif
