#ifndef MYSQL_H
#define MYSQL_H
#include<mysql/mysql.h>
class Mysql
{
public:
	Mysql();
	~Mysql();
	MYSQL* _mpcon;
	MYSQL_RES* _mp_res;
	MYSQL_ROW _mp_row;
};
#endif
