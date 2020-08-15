#ifndef VIEW_H
#define VIEW_H
#include <iostream>
#include <string>
using namespace std;
class View
{
	public:
		virtual void process(int fd,char* json){}
		virtual void response(){}
	private:
		int _fd;
		string _str;
};
#endif
