#include"login.h"
#include "mysql.h"
#include <string>
#include <string.h>
#include <json/json.h>
#include <mysql/mysql.h>
#include <errno.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
extern Mysql Mysql_sever;

void View_Login::process(int fd,char* json)
{
	_fd = fd;
	//���� json
	Json::Value root;
	Json::Reader read;

	read.parse(json,root);
	//name    pw
	string name = root["name"].asString();
	string pw = root["pw"].asString();
	
	//�����ݿ��в���name�������Ƿ���ȷ
	//  "select * from user where name='zhangsan' and pw='1111111';"
	char cmd[100] = "SELECT * FROM user WHERE name='";
	strcat(cmd,name.c_str());
	strcat(cmd,"' and pw='");
	strcat(cmd,pw.c_str());
	strcat(cmd,"';");

	/* 
	 *执行数据库命令之前，先    将这条sql语句进行hash运算（MD5/SHA...），以hash后的值作为key，用libmemcached命令在memcached服务器中
     *查找这个key对应的value，如果找到那么就直接返回value而不用访问数据库。 如果没有找到，那么接下来访问数据库，得到数据后先将数据
     *存放在memcached中，然后再将数据返回给客户端。
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
	else//�Ƚ����û����뵽online�У�Ȼ���ж��Ƿ�������ͻ��˶�Ӧ��������Ϣ�����û�о͸��ͻ��˻ظ���½�ɹ�����Ϣ������о͸��ͻ��˻ظ�������Ϣ���ܹ�����������Ϣ���Ѿ�������һ������ͻ��ܹ���½�ɹ�����Ϣ��
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
		_str = "login second";//����,������������������������
		//�������������������
		return ;
	}
		char tmp[6];
		sprintf(tmp,"%d",_fd);
		char cmd1[100] = "insert into online VALUE('";
		strcat(cmd1,tmp);
		strcat(cmd1,"','");
		strcat(cmd1,name.c_str());
		strcat(cmd1,"');");
		
		if(mysql_real_query(Mysql_sever._mpcon,cmd1,strlen(cmd1)))//��½�ɹ��󽫸��û�ע����online���У�insert��
		{
			cerr<<"insert fail;errno:"<<errno<<endl;
			return ;
		}
		//�鿴�Ƿ�������ͻ��˵�������Ϣ��offline(myname,youname,message)
		//ѡ���������������ͻ������е�������Ϣ
	   char cmd2[100] = "select * from offline where myname='";
	   strcat(cmd2,name.c_str());
	   strcat(cmd2,"';");
	   if(mysql_real_query(Mysql_sever._mpcon,cmd2,strlen(cmd2)))
		{
			cerr<<"insert fail;errno:"<<errno<<endl;
			return ;
		}
	   Mysql_sever._mp_res = mysql_store_result(Mysql_sever._mpcon);

	   Mysql_sever._mp_row = mysql_fetch_row(Mysql_sever._mp_res);//�ж�������Ϣ�б����Ƿ�������ͻ��˵�������Ϣ��
	   if(Mysql_sever._mp_row == NULL)//û��������Ϣ
	   {
		   _str= "login success";
	   }
	   else//��������Ϣ
	   {
		    char tmp[1024] = {0};//���ڴ��������Ϣ��������Ϣ�Ĵ�Ÿ�ʽΪ youname#message$youname#message$youname#message.........
		   //Mysql_sever._mp_row[0]---myname     Mysql_sever._mp_row[1]---youname    Mysql_sever._mp_row[2]---message
			strcpy(tmp,Mysql_sever._mp_row[1]);
			strcat(tmp,"#");
			strcat(tmp,Mysql_sever._mp_row[2]);
			Mysql_sever._mp_row = mysql_fetch_row(Mysql_sever._mp_res);
		   while(Mysql_sever._mp_row != NULL)//ѭ������������Ϣ���Ҽ��뵽_str�д洢������
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

	   Mysql_sever._mp_row = mysql_fetch_row(Mysql_sever._mp_res);//�ж�������Ϣ�б����Ƿ�������ͻ��˵�������Ϣ��
		if(Mysql_sever._mp_row != NULL)//������ͻ��˵�������Ϣ��ô��Ҫ��ȡ����֮���Ҫɾ�������ݿ�������ͻ��˵�������Ϣ��
	   {

		   char buf[128] = {0};
		   strcpy(buf,Mysql_sever._mp_row[0]);
		   char cmd4[100] = "delete from offline where myname='";
		   strcat(cmd4,name.c_str());
	       strcat(cmd4,"';");
	       if(mysql_real_query(Mysql_sever._mpcon,cmd4,strlen(cmd4)))//ɾ��������û������ݿ��е�����������Ϣ��
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
	//��ע��ɹ����Ľ�������͸��ͻ���
	send(_fd,_str.c_str(),strlen(_str.c_str()),0);
}
