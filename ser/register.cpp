#include"register.h"
#include"mysql.h"
#include <string>
#include <jsoncpp/json/json.h>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <iostream>
#include <arpa/inet.h>
#include <stdio.h>
using namespace std;
extern Mysql Mysql_sever;

void View_register::process(int fd,char* json)
{
	_fd = fd;
	//解析 json
	Json::Value root;
	Json::Reader read;
	
	read.parse(json,root);
	//name    pw
	string name = root["name"].asString();
	string pw = root["pw"].asString();
	
	//在数据库中查找name有没有重复
	char cmd[100] = "SELECT * FROM user WHERE name='";
	strcat(cmd,name.c_str());
	strcat(cmd,"';");
	
	if(mysql_real_query(Mysql_sever._mpcon,cmd,strlen(cmd)))
	{
		cerr<<"select fail;errno:"<<errno<<endl;
		return ;
	}
	Mysql_sever._mp_res = mysql_store_result(Mysql_sever._mpcon);

	Mysql_sever._mp_row = mysql_fetch_row(Mysql_sever._mp_res);

	if(Mysql_sever._mp_row == NULL)//说明这个用户名是没有注册过的用户，可以将客户端发过来的用户进行注册。
	{
		//将name pw 加入到数据库的user表
		char cmd1[100] = "insert into user VALUE('";
		strcat(cmd1,name.c_str());
		strcat(cmd1,"','");
		strcat(cmd1,pw.c_str());
		strcat(cmd1,"');");
		
		if(mysql_real_query(Mysql_sever._mpcon,cmd1,strlen(cmd1)))
		{
			cerr<<"insert fail;errno:"<<errno<<endl;
			return ;
		}
		_str = "register success";
	}
	else//说明这个用户已经注册过，不能再注册了，注册失败。
	{
		_str= "register fail";
	}
}

void View_register::response()
{	
	//将注册成功与否的结果，发送给客户端
	send(_fd,_str.c_str(),strlen(_str.c_str()),0);
}
