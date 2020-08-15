#ifndef TALK_TO_GROUP
#define TALK_TO_GROUP
#include"View.h"
class View_talk_to_group : public View
{
public:
	void process(int fd,char* json);
	void response();
private:
	int _fd;
	string _str;
};

#endif
