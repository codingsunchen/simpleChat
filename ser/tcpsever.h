#ifndef TCPSEVER_H
#define TCPSEVER_H
#include <iostream>
#include <vector>
#include <map>
#include <event.h>
#include "pthread.h"
using namespace std;
class Array
{
	public:
		int arr[2];
		Array(int (&a)[2])
		{
			arr[0] = a[0];
			arr[1] = a[1];
		}
		int& operator[](int i)
		{
			return arr[i];
		}
};

class Tcpserver
{
	public:
		//构造函数将通信的监听套接字listen_sockfd创建好。创建libevent
		Tcpserver(char *ip,short port,int pth_num);
		//~Tcpserver();服务器不许要析构
		void run();//让服务器跑起来，去接受客户端的连接，并把新的连接交给压力最小的子线程。
	private:
		int _listen_fd;//监听套接字
		int _pth_num;//启动的线程的个数
		struct event_base *_base;//libevent
		vector<Array> _socket_pair_vec;//socket pair vector
		vector<Pthread*> _pthread;//pthread vector
		map<int,int> _pth_work_num_map;//用于和子线程交流的fd+对应子线程监听的个数

		void get_socket_pair();
		void get_pthread();


		friend void listen_cb(int fd,short event,void* arg);
		friend void sock_pair_cb(int fd,short event,void* arg);
};

//class Pthread
//{
//	public:
//		Pthread(int socketpairfd);//构造一个线程的时候，就将socketpair中的一个fd交给子线程，用于与主线程之间进行通信。
//		~Pthread();
//	private:
//		int socketpair_fd;//用于与主线程交流的fd;
//		struct event_base* _base;//子线程的libevent,每次主线程通过sockpair_fd交给子线程一个客户端fd时，子线程就将该fd构造成事件加入到自己的libevent中监听起来，
//		                        //若该时间发生了就去调用相应的回调函数进行处理。
//		int listen_cli_num;//该子线程现在所监听客户端套接子的总个数，用于与主线程通信时，传递给主线程让主线程添加到map表中进行各个子线程的压力检查。
//		friend void cli_cb(int fd,short event,void *arg);//子线程监听客户端套接子时的回调函数。
//		friend void socketpair_cb(int fd,short event,void *arg);//子线程监听与主线程进行通信时的套接子的回调函数。
//};

#endif 
