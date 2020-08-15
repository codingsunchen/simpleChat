#include "talk_to_one.h"
#include "mysql.h"
#include "public.h"
#include <string>
#include <string.h>
#include <json/json.h>
#include <mysql/mysql.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <typeinfo>
extern Mysql Mysql_sever;

void View_talk_to_one::process(int fd,char* json)
{
	//cout<<"begin talk_to_one process()"<<endl;
	_fd = fd;
	Json::Value val;
	val["reason_type"] = MSG_TYPE_TALK_TO_ONE;
	Json::Value root;
	Json::Reader read;
	read.parse(json,root);
	string youname = root["youname"].asString();//youname¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿
	val["youname"] = youname.c_str();
	string myname = root["name"].asString();//myname¿¿¿¿¿¿¿¿
	string message = root["message"].asString();
	//  "select * from online where name='zhangsan';"
	char cmd[100] = "SELECT * FROM online where name='";
	strcat(cmd,youname.c_str());
	strcat(cmd,"';");

	if(mysql_real_query(Mysql_sever._mpcon,cmd,strlen(cmd)))
	{
		cerr<<"select fail;errno:"<<errno<<endl;
		return ;
	}
	Mysql_sever._mp_res = mysql_store_result(Mysql_sever._mpcon);

	if(Mysql_sever._mp_row = mysql_fetch_row(Mysql_sever._mp_res))//¿¿¿¿¿¿
	{
		char buff[1024] = {0};
		strcpy(buff,myname.c_str());
		strcat(buff,"specak::");
		strcat(buff,message.c_str());
		Json::Value mes;
		mes["reason_type"] = 123456;
		mes["message"] = buff;
		//cout<<"!!!!!!!!!!!!!"<<Mysql_sever._mp_row[0]<<endl;
		//cout<<typeid(Mysql_sever._mp_row[0]).name()<<endl;
		if(-1==send(atoi(Mysql_sever._mp_row[0]),mes.toStyledString().c_str(),strlen(mes.toStyledString().c_str()),0))
		{
			perror("send error:");
		}//¿¿¿¿¿¿¿¿¿¿
		val["IsSuccess"] = "talk_to_one success";
	}
	else//¿¿¿¿¿¿¿
	{
		//"select * from user where name='zhangsan';"
		char cmd1[100] = "SELECT * FROM user where name='";
		strcat(cmd1,youname.c_str());
		strcat(cmd1,"';");
		if(mysql_real_query(Mysql_sever._mpcon,cmd1,strlen(cmd1)))
		{
			cerr<<"select fail;errno:"<<errno<<endl;
			return ;
		}
		Mysql_sever._mp_res = mysql_store_result(Mysql_sever._mpcon);
	
		if(Mysql_sever._mp_row = mysql_fetch_row(Mysql_sever._mp_res))//¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿offline
		{
			//"insert into offline VALUE('myname','youname','message');"
			char cmd2[1024] = "insert into offline VALUE('";
			strcat(cmd2,youname.c_str());
			strcat(cmd2,"','");
			strcat(cmd2,myname.c_str());
			strcat(cmd2,"','");
			strcat(cmd2,message.c_str());
			strcat(cmd2,"');");
			if(mysql_real_query(Mysql_sever._mpcon,cmd2,strlen(cmd2)))
			{
				cerr<<"select fail;errno:"<<errno<<endl;
				return ;
			}
			val["IsSuccess"] = "talk_to_one success";
		}
		else//¿¿¿¿¿¿¿¿¿¿¿¿¿
		{
			val["IsSuccess"] = "talk_to_one fail";
		}
	}
	_str = val.toStyledString();
}

void View_talk_to_one::response()
{	
	//½«×¢²á³É¹¦Óë·ñµÄ½á¹û£¬·¢ËÍ¸ø¿Í»§¶Ë
	send(_fd,_str.c_str(),strlen(_str.c_str()),0);
}
