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
	//½âÎö json
	Json::Value root;
	Json::Reader read;

	read.parse(json,root);
	//name    pw
	string name = root["name"].asString();
	string pw = root["pw"].asString();
	
	//ÔÚÊı¾İ¿âÖĞ²éÕÒnameºÍÃÜÂëÊÇ·ñÕıÈ·
	//  "select * from user where name='zhangsan' and pw='1111111';"
	char cmd[100] = "SELECT * FROM user WHERE name='";
	strcat(cmd,name.c_str());
	strcat(cmd,"' and pw='");
	strcat(cmd,pw.c_str());
	strcat(cmd,"';");

	/* 
	 *æ‰§è¡Œæ•°æ®åº“å‘½ä»¤ä¹‹å‰ï¼Œå…ˆ    å°†è¿™æ¡sqlè¯­å¥è¿›è¡Œhashè¿ç®—ï¼ˆMD5/SHA...ï¼‰ï¼Œä»¥hashåçš„å€¼ä½œä¸ºkeyï¼Œç”¨libmemcachedå‘½ä»¤åœ¨memcachedæœåŠ¡å™¨ä¸­
     *æŸ¥æ‰¾è¿™ä¸ªkeyå¯¹åº”çš„valueï¼Œå¦‚æœæ‰¾åˆ°é‚£ä¹ˆå°±ç›´æ¥è¿”å›valueè€Œä¸ç”¨è®¿é—®æ•°æ®åº“ã€‚ å¦‚æœæ²¡æœ‰æ‰¾åˆ°ï¼Œé‚£ä¹ˆæ¥ä¸‹æ¥è®¿é—®æ•°æ®åº“ï¼Œå¾—åˆ°æ•°æ®åå…ˆå°†æ•°æ®
     *å­˜æ”¾åœ¨memcachedä¸­ï¼Œç„¶åå†å°†æ•°æ®è¿”å›ç»™å®¢æˆ·ç«¯ã€‚
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
	else//ÏÈ½«¸ÃÓÃ»§¼ÓÈëµ½onlineÖĞ£¬È»ºóÅĞ¶ÏÊÇ·ñÓĞÕâ¸ö¿Í»§¶Ë¶ÔÓ¦µÄÀëÏßÏûÏ¢£¬Èç¹ûÃ»ÓĞ¾Í¸ø¿Í»§¶Ë»Ø¸´µÇÂ½³É¹¦µÄÏûÏ¢£¬Èç¹ûÓĞ¾Í¸ø¿Í»§¶Ë»Ø¸´ÀëÏßÏûÏ¢£¬ÄÜ¹»·¢ËÍÀëÏßÏûÏ¢¾ÍÒÑ¾­Òşº¬ÁËÒ»¸öÕâ¸ö¿Í»§ÄÜ¹»µÇÂ½³É¹¦µÄÏûÏ¢¡£
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
		_str = "login second";//¿¿¿¿,¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿
		//¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿
		return ;
	}
		char tmp[6];
		sprintf(tmp,"%d",_fd);
		char cmd1[100] = "insert into online VALUE('";
		strcat(cmd1,tmp);
		strcat(cmd1,"','");
		strcat(cmd1,name.c_str());
		strcat(cmd1,"');");
		
		if(mysql_real_query(Mysql_sever._mpcon,cmd1,strlen(cmd1)))//µÇÂ½³É¹¦ºó½«¸ÃÓÃ»§×¢²áÔÚonline±íÖĞ£¨insert£©
		{
			cerr<<"insert fail;errno:"<<errno<<endl;
			return ;
		}
		//²é¿´ÊÇ·ñÓĞÕâ¸ö¿Í»§¶ËµÄÀëÏßÏûÏ¢¡£offline(myname,youname,message)
		//Ñ¡Ôñ³öÀ´ÁËÊôÓÚÕâ¸ö¿Í»§¶ËËùÓĞµÄÀëÏßÏûÏ¢
	   char cmd2[100] = "select * from offline where myname='";
	   strcat(cmd2,name.c_str());
	   strcat(cmd2,"';");
	   if(mysql_real_query(Mysql_sever._mpcon,cmd2,strlen(cmd2)))
		{
			cerr<<"insert fail;errno:"<<errno<<endl;
			return ;
		}
	   Mysql_sever._mp_res = mysql_store_result(Mysql_sever._mpcon);

	   Mysql_sever._mp_row = mysql_fetch_row(Mysql_sever._mp_res);//ÅĞ¶ÏÀëÏßÏûÏ¢ÁĞ±íÖĞÊÇ·ñÓĞÕâ¸ö¿Í»§¶ËµÄÀëÏßÏûÏ¢¡£
	   if(Mysql_sever._mp_row == NULL)//Ã»ÓĞÀëÏßÏûÏ¢
	   {
		   _str= "login success";
	   }
	   else//ÓĞÀëÏßÏûÏ¢
	   {
		    char tmp[1024] = {0};//ÓÃÓÚ´æ·ÅÀëÏßÏûÏ¢£¬ÀëÏßÏûÏ¢µÄ´æ·Å¸ñÊ½Îª youname#message$youname#message$youname#message.........
		   //Mysql_sever._mp_row[0]---myname     Mysql_sever._mp_row[1]---youname    Mysql_sever._mp_row[2]---message
			strcpy(tmp,Mysql_sever._mp_row[1]);
			strcat(tmp,"#");
			strcat(tmp,Mysql_sever._mp_row[2]);
			Mysql_sever._mp_row = mysql_fetch_row(Mysql_sever._mp_res);
		   while(Mysql_sever._mp_row != NULL)//Ñ­»·¶Á³öÀëÏßÏûÏ¢²¢ÇÒ¼ÓÈëµ½_strÖĞ´æ´¢ÆğÀ´¡£
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

	   Mysql_sever._mp_row = mysql_fetch_row(Mysql_sever._mp_res);//ÅĞ¶ÏÀëÏßÏûÏ¢ÁĞ±íÖĞÊÇ·ñÓĞÕâ¸ö¿Í»§¶ËµÄÀëÏßÏûÏ¢¡£
		if(Mysql_sever._mp_row != NULL)//ÓĞÕâ¸ö¿Í»§¶ËµÄÀëÏßÏûÏ¢ÄÇÃ´¾ÍÒª¶ÁÈ¡³öÀ´Ö®ºó¾ÍÒªÉ¾³ıµôÊı¾İ¿âÖĞÕâ¸ö¿Í»§¶ËµÄÀëÏßÏûÏ¢¡£
	   {

		   char buf[128] = {0};
		   strcpy(buf,Mysql_sever._mp_row[0]);
		   char cmd4[100] = "delete from offline where myname='";
		   strcat(cmd4,name.c_str());
	       strcat(cmd4,"';");
	       if(mysql_real_query(Mysql_sever._mpcon,cmd4,strlen(cmd4)))//É¾³ıµôÕâ¸öÓÃ»§ÔÚÊı¾İ¿âÖĞµÄËùÓĞÀëÏßÏûÏ¢¡£
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
	//½«×¢²á³É¹¦Óë·ñµÄ½á¹û£¬·¢ËÍ¸ø¿Í»§¶Ë
	send(_fd,_str.c_str(),strlen(_str.c_str()),0);
}
