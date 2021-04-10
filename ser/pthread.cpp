#include"pthread.h"
#include "control.h"
#include "public.h"
#include <assert.h>
#include <event.h>
#include <jsoncpp/json/json.h>
#include <stdio.h>
#include <stdlib.h>
#include <map>
#include <iostream>
#include <string.h>
#include <functional>
#include <algorithm>
using namespace std;

extern Control control_sever;
void* pthread_run(void *arg);
void socketpair_cb(int fd,short event,void *arg);//arg应该接收的是子线程的对象this指针
Pthread::Pthread(int sock_fd)
{
	_socketpair_fd = sock_fd;//socketpair的1端

	//启动线程
	int n = pthread_create(&_pthread,NULL,pthread_run,this);
	//assert(n==0);
	if(0!=n)
	{
		perror("pthread_create error:");
		return ;
	}
}

void* pthread_run(void *arg)
{
	Pthread *This = (Pthread*)arg;
	//将sock_pair_1加入到libevent  sock_pair_1_cb()
	This->_base = event_base_new();
	//创建事件，绑定socketpair_fd的回调函数(socketpair_cb)
	struct event* listen_event = event_new(This->_base,This->_socketpair_fd,EV_READ|EV_PERSIST,socketpair_cb,This);
	assert(listen_event != NULL);
	event_add(listen_event,NULL);//将listen_fd加入到主线程的libevent中监听起来。若有事件发生就去调用回调函数。
	event_base_dispatch(This->_base);
}

//对客户端请求的处理。      将客户端发过来的请求Json进行解析，得到消息类型，对不同类型的消息进行不同的处理方式。
//将消息交给controler进行管理    典型的观察者监听者模式（MVC），所有的消息都对应一个对象，每个对象都把自己关注的消息注册在
//controler中，然后controler对这些消息进行监听，当某种消息发生时，就会自动调用对象在这里注册时指定的处理方式。
void cli_cb(int clifd,short event,void *arg)//arg应该对应Pthread对象的指针this
{
	//cout<<"cli_cb()"<<endl;
	Pthread *This = (Pthread*)arg;
	char buff[2048] = {0};
	if(-1 == recv(clifd,buff,2047,0))
	{
		perror("cli_cb recv fail:");
		return ;
	}
	Json::Value root;
	Json::Reader read;
	read.parse(buff,root);
	//如果发现发过来的是客户端退出消息,那么在这个回调函数中救直接将_event_map表中存储这个客户端所生成的事件删除掉。
	//因为后期也不会再用到了。
	if(MSG_TYPE_EXIT == root["reason_type"].asInt())
	{
		event_free(This->_event_map[clifd]);
		This->_event_map.erase(This->_event_map.find(clifd));
	}
	//cout<<"begin control process"<<endl;
	control_sever.process(clifd,buff);
	//cout<<"success control process"<<endl;
}


//socketpair专门用于将主线程交给的客户端套接子监听起来，事件发生时调用回调函数进行处理。
void socketpair_cb(int fd,short event,void *arg)//arg应该接收的是子线程的对象this指针
{
	//struct event_base *_base = (struct event_base*)arg;
	Pthread *This = (Pthread*)arg;
	//接受主线程传送过来的客户端套接字
	char buff[16] = {0};
	if(-1 == recv(This->_socketpair_fd,buff,15,0))
	{
		perror("recv error:");
		return ;
	}
	int clifd = atoi(buff);//得到主线程传给子线程的客户端描述符。
	//将客户端描述符构造为事件，交给子线程的libevent监听起来
	struct event* listen_event = event_new(This->_base,clifd,EV_READ|EV_PERSIST,cli_cb,This);
	assert(listen_event != NULL);
	event_add(listen_event,NULL);

	//将这个客户端事件记录在map表中
	This->_event_map.insert(make_pair(clifd,listen_event));

	//回复主线程，现在子线程中监听了多少个客户端的事件了。
	memset(buff,0,16);
	int listen_num = This->_event_map.size();
	sprintf(buff,"%d",listen_num);
	send(This->_socketpair_fd,buff,strlen(buff),0);
}


/*
sock_pair_1_cb(int fd,short event,void *arg)
{
	//recv -> clien_fd
	
	//将client_fd加入到libevent     client_cb()
	
	//给主线程回复当前监听的客户端数量

}

client_cb(int fd,short event,void *arg)
{
	//recv  ->buff
	
	//将buff发给control
	control_sever.process(fd,buff)
}
*/

