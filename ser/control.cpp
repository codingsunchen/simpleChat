#include"control.h"
#include "View.h"
#include "login.h"
#include "register.h"
#include "exit.h"
#include "get_list.h"
#include "talk_to_one.h"
#include "talk_to_group.h"
#include "public.h"
#include <iostream>
#include <jsoncpp/json/json.h>
#include <map>
#include <functional>
#include <algorithm>
using namespace std;
Control::Control()
{
	_map.insert(make_pair(MSG_TYPE_LOGIN,new View_Login));
	_map.insert(make_pair(MSG_TYPE_REGISTER,new View_register));

	_map.insert(make_pair(MSG_TYPE_TALK_TO_ONE,new View_talk_to_one));
	_map.insert(make_pair(MSG_TYPE_TALK_TO_GROUP,new View_talk_to_group));
	_map.insert(make_pair(MSG_TYPE_GET_LIST,new View_Getlist));
	_map.insert(make_pair(MSG_TYPE_EXIT,new View_Exit));
}

void Control::process(int fd,char* json)
{
	//解析json，获取消息类型
	Json::Value root;
	Json::Reader read;

	read.parse(json,root);
	
	MSG_TYPE message = (MSG_TYPE)root["reason_type"].asInt();//得到消息类型

	//根据消息类型在map中查找 不牵扯找不到消息类型的情况，因为事先已经规定好所有出现的消息类型只能有哪些了
	map<int,View*>::iterator it = _map.find(message);
	//判断是否找到,按照设计模式来说都能够找到，所以不用判断
	
	it->second->process(fd,json);//让control绑定的该消息的对象去处理。将消息全部交给对象。
	it->second->response();//让绑定的对象去回复。
}

Control control_sever;
//MVC 模式， 子线程将接收到的消息交给control，control得到消息后它会根据消息的类型绑定对应的消息处理对象，如何处理control不管，
//是由绑定的处理对象来管的。
