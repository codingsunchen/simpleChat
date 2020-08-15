#ifndef FUN_H
#define FUN_H
#include <string>
#include <iostream>
using namespace std;
//客户端发给服务器的所有请求，客户端都不需要关心服务器处理这个请求的方式，是由服务器的主线程还是子线程处理客户端都不关心。
//设计时，服务器方面将客户端的所有请求全部都交给服务器的子线程处理，服务器的主线程只是监听有没有客户端要和自己连接，
//服务器的主线程只处理链接，并管理所有的子线程。
//规定客户端发给服务器的所有请求都是Json包的形式，这样服务器那边就可以对Json包进行解析，得到客户端想要干什么。：
void do_register(int fd);

void do_login(int fd);

void do_get_list(int fd);

void do_talk_to_one(int fd,string);

void do_talk_to_group(int fd,string);

void do_exit(int fd);
 
#endif
