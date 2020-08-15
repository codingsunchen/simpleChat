#ifndef PTHREAD_H
#define PTHREAD_H
#include "exit.h"
#include <iostream>
#include <map>
#include <pthread.h>

using namespace std;
class Pthread
{
public:
	Pthread(int socketpair_fd);//构造一个线程的时候，就将socketpair中的一个fd交给子线程，用于与主线程之间进行通信。
	//~Pthread();

private:
	int _socketpair_fd;//sock_pair    (1)  //用于与主线程交流的fd;
	//libevent//子线程的libevent,每次主线程通过sockpair_fd交给子线程一个客户端fd时，子线程就将该fd构造
			  //成事件加入到自己的libevent中监听起来，若该时间发生了就去调用相应的回调函数进行处理。
	struct event_base* _base;

	//可以有map表计算得到listen_num所以不用再定义listen_num成员变量。  map.size();
	//主要存的是监听客户端套接字的事件。   socketpair的事件其实可以不用存放，因为释放socketpair上的事件说明服务器有错误，
	//服务器崩溃，socketpair的事件属于服务器整个运行期间都要拥有的资源，所以不用加入到map表中。  
	map<int,struct event*> _event_map;//存放事件的map表 当客户端下线的时候，要将该客户端clifd对应的事件释放掉。
	pthread_t _pthread;//线程描述符   一个线程在创建的时候对应一个线程描述符，在子线程的构造函数中要使用。

	friend void socketpair_cb(int fd,short event,void *arg);//子线程监听与主线程进行通信时的套接子的回调函数。
	friend void cli_cb(int fd,short event,void *arg);//子线程监听客户端套接子时的回调函数。
	friend void* pthread_run(void *arg);
	friend class View_Exit;
};

#endif
