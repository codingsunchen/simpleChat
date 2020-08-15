#include"get_list.h"
#include "mysql.h"
#include "public.h"
#include <string>
#include <string.h>
#include <json/json.h>
#include <mysql/mysql.h>
#include <errno.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
extern Mysql Mysql_sever;

void View_Getlist::process(int fd,char* json)
{
	_fd = fd;	
	//将数据库中注册过的用户名也就是user表中的所有用户名读取出来存在一个字符串中
	//字符串中存储的用户名格式为：name#name#name#name#name#name........
	Json::Value val;
	val["reason_type"] = MSG_TYPE_GET_LIST;
	char buff[1024] = {0};

	//  "select * from user;
	char cmd[100] = "SELECT * FROM user;";
	
	if(mysql_real_query(Mysql_sever._mpcon,cmd,strlen(cmd)))
	{
		cerr<<"select fail;errno:"<<errno<<endl;
		return ;
	}
	Mysql_sever._mp_res = mysql_store_result(Mysql_sever._mpcon);

	//没有注册过任何用户，不存在这种情况，因为能够调用到get_list函数，说明至少有一个用户注册过，他才能登陆上来选择该服务。
	//if(Mysql_sever._mp_row == NULL)//不存在该种情况。

	while(Mysql_sever._mp_row = mysql_fetch_row(Mysql_sever._mp_res))
	{
		strcat(buff,Mysql_sever._mp_row[0]);
		strcat(buff,"#");
	}
	buff[strlen(buff)-1] = '\0';
	val["list"] = buff;
	_str = val.toStyledString();
}

void View_Getlist::response()
{	
	//将注册成功与否的结果，发送给客户端
	send(_fd,_str.c_str(),strlen(_str.c_str()),0);
}
