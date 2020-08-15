#ifndef LOGIN
#define LOGIN
#include"View.h"
class View_Login : public View
{
public:
	void process(int fd,char* json);
	void response();
private:
	int _fd;
	string _str;
};

#endif
