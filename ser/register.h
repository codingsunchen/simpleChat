#ifndef REGISTER
#define REGISTER
#include "View.h"
class View_register:public View
{
public:
	void process(int fd,char* json);
	void response();
private:
	int _fd;
	string _str;
};

#endif
