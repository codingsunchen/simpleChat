#include"exit.h"
#include "mysql.h"
#include "pthread.h"
#include <string>
#include <string.h>
#include <json/json.h>
#include <event.h>
#include <iostream>
#include <map>
#include <mysql/mysql.h>
#include <errno.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
using namespace std;
extern Mysql Mysql_sever;

void View_Exit::process(int fd,char* json)
{
	_fd = fd;
	
	char cmd[100] = "delete from online where clifd=";
	char bu[16] = {0};
	sprintf(bu,"%d",_fd);
	strcat(cmd,bu);
	strcat(cmd,";");
	if(mysql_real_query(Mysql_sever._mpcon,cmd,strlen(cmd)))
	{
		cerr<<"delete fail;errno:"<<errno<<endl;
		return ;
	}
}

void View_Exit::response()
{	
	//¿¿¿¿¿¿¿¿¿¿¿¿¿online¿¿¿¿¿¿¿¿¿¿¿
}
