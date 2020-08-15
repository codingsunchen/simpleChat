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
	//�����ݿ���ע������û���Ҳ����user���е������û�����ȡ��������һ���ַ�����
	//�ַ����д洢���û�����ʽΪ��name#name#name#name#name#name........
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

	//û��ע����κ��û��������������������Ϊ�ܹ����õ�get_list������˵��������һ���û�ע����������ܵ�½����ѡ��÷���
	//if(Mysql_sever._mp_row == NULL)//�����ڸ��������

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
	//��ע��ɹ����Ľ�������͸��ͻ���
	send(_fd,_str.c_str(),strlen(_str.c_str()),0);
}
