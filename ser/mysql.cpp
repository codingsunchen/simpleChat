#include"mysql.h"
#include <iostream>
#include <errno.h>
using namespace std;

Mysql::Mysql()
{
	_mpcon = mysql_init((MYSQL*)0);
	if(NULL == _mpcon)
	{
		cerr<<"mpcon = NULL"<<endl;
		return ;
	}
	//链接数据库
	if(!mysql_real_connect(_mpcon,"127.0.0.1","root","123456",NULL,3306,NULL,0))//成功返回0
	{
		cerr<<"mysql connect fail;errno:"<<errno<<endl;
		return ;
	}
	//选择数据库

	if(mysql_select_db(_mpcon,"chat"))
	{
		cerr<<"database select fail;errno:"<<errno<<endl;
		return ;
	}
}

Mysql::~Mysql()
{
	if(NULL != _mp_res)
	{
		mysql_free_result(_mp_res);
	}

	mysql_close(_mpcon);
}


Mysql Mysql_sever;
