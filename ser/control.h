#ifndef CONTROL_H
#define CONTROL_H
#include "View.h"
#include <iostream>
#include <map>
using namespace std;
class Control
{
public:
	Control();
	//~Control();
	void process(int fd,char* json);
private:
	map<int,View*> _map;
};
#endif
