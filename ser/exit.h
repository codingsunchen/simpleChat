#ifndef EXIT
#define EXIT
#include"View.h"
class View_Exit : public View
{
public:
	void process(int fd,char* json);
	void response();
private:
	int _fd;
	//string _str;
};

#endif