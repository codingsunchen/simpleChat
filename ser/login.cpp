#include"login.h"
#include "mysql.h"
#include <string>
#include <string.h>
#include <jsoncpp/json/json.h>
#include <mysql/mysql.h>
#include <errno.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
extern Mysql Mysql_sever;

void View_Login::process(int fd,char* json)
{
	_fd = fd;
	//解析 json
	Json::Value root;
	Json::Reader read;

	read.parse(json,root);
	//name    pw
	string name = root["name"].asString();
	string pw = root["pw"].asString();
	
	//在数据库中查找name和密码是否正确
	//  "select * from user where name='zhangsan' and pw='1111111';"
	char cmd[100] = "SELECT * FROM user WHERE name='";
	strcat(cmd,name.c_str());
	strcat(cmd,"' and pw='");
	strcat(cmd,pw.c_str());
	strcat(cmd,"';");

	/* 
	 *鎵ц鏁版嵁搴撳懡浠や箣鍓嶏紝鍏�    灏嗚繖鏉ql璇彞杩涜hash杩愮畻锛圡D5/SHA...锛夛紝浠ash鍚庣殑鍊间綔涓簁ey锛岀敤libmemcached鍛戒护鍦╩emcached鏈嶅姟鍣ㄤ腑
     *鏌ユ壘杩欎釜key瀵瑰簲鐨剉alue锛屽鏋滄壘鍒伴偅涔堝氨鐩存帴杩斿洖value鑰屼笉鐢ㄨ闂暟鎹簱銆� 濡傛灉娌℃湁鎵惧埌锛岄偅涔堟帴涓嬫潵璁块棶鏁版嵁搴擄紝寰楀埌鏁版嵁鍚庡厛灏嗘暟鎹�
     *瀛樻斁鍦╩emcached涓紝鐒跺悗鍐嶅皢鏁版嵁杩斿洖缁欏鎴风銆�
	*/
	if(mysql_real_query(Mysql_sever._mpcon,cmd,strlen(cmd)))
	{
		cerr<<"select fail;errno:"<<errno<<endl;
		return ;
	}
	Mysql_sever._mp_res = mysql_store_result(Mysql_sever._mpcon);

	Mysql_sever._mp_row = mysql_fetch_row(Mysql_sever._mp_res);

	if(Mysql_sever._mp_row == NULL)
	{
		_str = "login fail";
	}
	else//先将该用户加入到online中，然后判断是否有这个客户端对应的离线消息，如果没有就给客户端回复登陆成功的消息，如果有就给客户端回复离线消息，能够发送离线消息就已经隐含了一个这个客户能够登陆成功的消息。
	{
		
	char cmd5[100] = "SELECT * FROM online WHERE name='";
	strcat(cmd5,name.c_str());
	strcat(cmd5,"';");
	
	if(mysql_real_query(Mysql_sever._mpcon,cmd5,strlen(cmd5)))
	{
		cerr<<"select fail;errno:"<<errno<<endl;
		return ;
	}
	Mysql_sever._mp_res = mysql_store_result(Mysql_sever._mpcon);

	Mysql_sever._mp_row = mysql_fetch_row(Mysql_sever._mp_res);

	if(Mysql_sever._mp_row != NULL)
	{
		_str = "login second";//靠靠,靠靠靠靠靠靠靠靠靠靠靠靠
		//靠靠靠靠靠靠靠靠靠�
		return ;
	}
		char tmp[6];
		sprintf(tmp,"%d",_fd);
		char cmd1[100] = "insert into online VALUE('";
		strcat(cmd1,tmp);
		strcat(cmd1,"','");
		strcat(cmd1,name.c_str());
		strcat(cmd1,"');");
		
		if(mysql_real_query(Mysql_sever._mpcon,cmd1,strlen(cmd1)))//登陆成功后将该用户注册在online表中（insert）
		{
			cerr<<"insert fail;errno:"<<errno<<endl;
			return ;
		}
		//查看是否有这个客户端的离线消息。offline(myname,youname,message)
		//选择出来了属于这个客户端所有的离线消息
	   char cmd2[100] = "select * from offline where myname='";
	   strcat(cmd2,name.c_str());
	   strcat(cmd2,"';");
	   if(mysql_real_query(Mysql_sever._mpcon,cmd2,strlen(cmd2)))
		{
			cerr<<"insert fail;errno:"<<errno<<endl;
			return ;
		}
	   Mysql_sever._mp_res = mysql_store_result(Mysql_sever._mpcon);

	   Mysql_sever._mp_row = mysql_fetch_row(Mysql_sever._mp_res);//判断离线消息列表中是否有这个客户端的离线消息。
	   if(Mysql_sever._mp_row == NULL)//没有离线消息
	   {
		   _str= "login success";
	   }
	   else//有离线消息
	   {
		    char tmp[1024] = {0};//用于存放离线消息，离线消息的存放格式为 youname#message$youname#message$youname#message.........
		   //Mysql_sever._mp_row[0]---myname     Mysql_sever._mp_row[1]---youname    Mysql_sever._mp_row[2]---message
			strcpy(tmp,Mysql_sever._mp_row[1]);
			strcat(tmp,"#");
			strcat(tmp,Mysql_sever._mp_row[2]);
			Mysql_sever._mp_row = mysql_fetch_row(Mysql_sever._mp_res);
		   while(Mysql_sever._mp_row != NULL)//循环读出离线消息并且加入到_str中存储起来。
		   {
			  strcat(tmp,"$");
			  strcat(tmp,Mysql_sever._mp_row[1]);
			  strcat(tmp,"#");
			  strcat(tmp,Mysql_sever._mp_row[2]);
			  Mysql_sever._mp_row = mysql_fetch_row(Mysql_sever._mp_res);
		   }
		   _str = tmp;
		   //cout<<_str<<endl;
		   //cout<<"offline cunchu success"<<endl;
	   }

	   char cmd3[100] = "select * from offline where myname='";
	   strcat(cmd3,name.c_str());
	   strcat(cmd3,"';");
	   if(mysql_real_query(Mysql_sever._mpcon,cmd3,strlen(cmd3)))
		{
			cerr<<"insert fail;errno:"<<errno<<endl;
			return ;
		}
	   Mysql_sever._mp_res = mysql_store_result(Mysql_sever._mpcon);

	   Mysql_sever._mp_row = mysql_fetch_row(Mysql_sever._mp_res);//判断离线消息列表中是否有这个客户端的离线消息。
		if(Mysql_sever._mp_row != NULL)//有这个客户端的离线消息那么就要读取出来之后就要删除掉数据库中这个客户端的离线消息。
	   {

		   char buf[128] = {0};
		   strcpy(buf,Mysql_sever._mp_row[0]);
		   char cmd4[100] = "delete from offline where myname='";
		   strcat(cmd4,name.c_str());
	       strcat(cmd4,"';");
	       if(mysql_real_query(Mysql_sever._mpcon,cmd4,strlen(cmd4)))//删除掉这个用户在数据库中的所有离线消息。
		   {
			  cerr<<"delete offline fail;errno:"<<errno<<endl;
			  return ;
		   }
    	}
		//cout<<"delete offline success"<<endl;
	}
}

void View_Login::response()
{	
	//将注册成功与否的结果，发送给客户端
	send(_fd,_str.c_str(),strlen(_str.c_str()),0);
}
