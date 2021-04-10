#include "talk_to_group.h"
#include "talk_to_one.h"
#include "mysql.h"
#include "public.h"
#include <string>
#include <string.h>
#include <jsoncpp/json/json.h>
#include <mysql/mysql.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <typeinfo>
#include <vector>
#include <iostream>
using namespace std;
extern Mysql Mysql_sever;

View_talk_to_one talk_to_one;
void View_talk_to_group::process(int fd,char* json)
{
	//cout<<"talk_to_group()"<<endl;
	//cout<<json<<endl;
	Json::Value root;
	Json::Reader read;
	read.parse(json,root);
	Json::Value val;
	val["reason_type"] = MSG_TYPE_TALK_TO_ONE;
	val["message"] = root["message"].asString().c_str();
	val["name"] = root["name"].asString().c_str();
	int size = strlen(root["othername"].asString().c_str());
	char buff[size+1];
	buff[size] = '\0';
	strcpy(buff,root["othername"].asString().c_str());
	vector<char*> vec;
	char *p = NULL;
	char *s = strtok_r(buff,"#",&p);
	while(s!=NULL)
	{
		vec.push_back(s);
		s = strtok_r(NULL,"#",&p);
	}
	int len = vec.size();//¿¿¿¿¿len¿¿?	
	for(int i=0;i<len;i++)
	{
		val["youname"] = vec[i];
		char tmp[strlen(val.toStyledString().c_str()+1)];
		strcpy(tmp,val.toStyledString().c_str());
		//cout<<tmp<<endl;
		talk_to_one.process(fd,tmp);
		talk_to_one.response();
	}
}
//talk_to_group¿response¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿process¿¿¿¿¿¿
void View_talk_to_group::response()
{	
}
