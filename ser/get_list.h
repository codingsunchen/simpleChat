#ifndef GETLIST
#define GETLIST
#include"View.h"
class View_Getlist : public View
{
public:
	void process(int fd,char* json);
	void response();
private:
	int _fd;
	string _str;
};

#endif