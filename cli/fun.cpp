#include <iostream>
#include <string.h>
#include <string>
#include <vector>
#include"fun.h"
#include"public.h"
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <jsoncpp/json/json.h>
using namespace std;
int flg = 0;

//根据服务器发过来的Json包判断是获取在线列表的响应，然后对发过的在线列表进行打印。
//规定服务器发的Json包中包含列表的内容全部在val["list"],中存储，每个名字之间用#进行相隔开(定义协议)
//将Json包的字符串val["list"]传给该函数。
static void get_list(const char *list)
{
	//不存在没有好友上线的情况，因为当本客户端登录成功以后，上线人员至少有你自己一个人，
	//所以服务器发过来的好友列表中至少有你自己的姓名。
	char buff[strlen(list)+1];
	for(int i=0;i<strlen(list);i++)
	{
		buff[i] = list[i];
	}
	buff[strlen(list)] = '\0';
	vector<char*> vec;
	char *p = NULL;
	char *s = strtok_r(buff,"#",&p);
	while(s!=NULL)
	{
		vec.push_back(s);
		s = strtok_r(NULL,"#",&p);
	}
	int len = vec.size();
	for(int i=0;i<len;i++)
	{
		cout<<i+1<<":"<<vec[i]<<endl;
	}
}
void get_talk(const char *IsSuccess, const char* name)
{
	if(strcmp(IsSuccess, "talk_to_one success") == 0)
	{
		cout<<"给"<<name<<"的消息发送成功"<<endl;
	}
	else
	{
		cout<<name<<"这个好友不存在，发送失败"<<endl;
	}
}
/*   服务器对于群聊消息的处理实质还是按照私聊的方式进行处理的，所以客户端不会接受到对群聊消息的回复。
void get_talk_group(const char *IsSuccess)
{
	if(strcmp(IsSuccess,"talk_to_group success") == 0)
	{
		cout<<"群发的消息发送成功"<<endl;
	}
	else
	{
		cout<<"群发成员中有未注册成员，请重新确定群发成员名单，发送失败"<<endl;
	}
}
*/

//规定在登录成功之后所有在客户端与服务器端传送的消息都是以Json包的形式传送。
//登录成功之前，消息不需要由子线程进行读取，所以登录之前所有的消息都以字符串的形式进行传输。
//登录成功后所有的消息都要由子线程进行接收，所以以Json包的形式传输这样方便子对不同消息进行解析。


//talk_to_one 和talk_to_group 成功与否，要由服务器给客户端回应的Json包进行判断。


//客户端子线程专门用于接受服务器发送过来的消息。把客户端与服务器通信的socket传给子进程，让子进程与服务器通信，
//接收服务器的消息。
//登录成功了以后，以后服务器发送给客户端的消息，全部都由客户端的子线程来接收。
//子线程做的事情就是接收服务器的消息并打印到客户端的屏幕上。
void *pthread_run(void *arg)
{
	int *p = (int*)arg;
	int sockfd = *p;
	char buff[2048] = {0};
	Json::Value root;	
	Json::Reader read;
	
	while(1)
	{
		//如果是自己给自己发那么就会出现Json包连包的问题。那么客户端这边接受到后只能解析buff中的第一个Json包。
		recv(sockfd,buff,2047,0);
		//cout<<buff<<endl;
		if(-1 == read.parse(buff,root))
		{
			perror("json prase fail:");
			return NULL;
		}
		Msg_Type select = (Msg_Type)root["reason_type"].asInt();
		switch(select)
		{
			//规定服务器发送列表时是把列表的名单存放在字符串中，在Json包的list选项中存放。回应获得在线人员类表的消息，
			//子线程接受到服务器回应的消息后，对消息解析将列表进行打印
			//私聊是否成功，服务器应该将是否成功和对方用户的姓名都打包在Json包中，
			//这样，在客户端就可以得到是给谁发的消息是否成功了。
			case REASON_TYPE_LIST:get_list(root["list"].asString().c_str()); flg=1;break;
			////规定服务器把成功与否的消息在Json的IsSuccess字符串中存放，回应talk_to_one是否成功的消息，若成功打印发送成功，若失败打印好友不存在发送失败
			case REASON_TYPE_TALK:get_talk(root["IsSuccess"].asString().c_str(),root["youname"].asString().c_str());break;
			//case REASON_TYPE_GROUP:get_talk_group(root["IsSuccess"].asString().c_str());break;//回应群发消息talk_to_group是否成功的消息，若成功打印发送成功，若失败打印好友不存在发送失败，服务器在处理的时候
											//可以根据客户端发送过来的群聊人员进行全部检查，如果发现只要有一个客户端给出的人员不存在，服务器就不进行处理，给客户端
											//发送失败消息。
			default: cout<<root["message"].asString()<<endl;//default 说明服务器发给客户端的是另一个客户端发给自己当前用户的消息。
		}
	}
}


//参数传入的是与服务器端进行通信的套接字
void do_register(int sockfd)
{
	//让用户输入name   pw
	string name;
	string pw;
	cout<<"请输入用户姓名:"<<endl;
	cin>>name;
	cout<<"请输入用户密码:"<<endl;
	cin>>pw;
	
	//JSON 打包数据，通过Json包把消息类型加上去，让服务器接收到这个包后可以知道客户端是想要干什么。
	Json::Value val;
	val["reason_type"] = REASON_TYPE_REGISTER;//消息类型,把Json包发给服务器后，由服务器进行解析消息的类型进行消息相应的处理。
	val["name"] = name.c_str();
	val["pw"] = pw.c_str();

	//发送到服务器
	send(sockfd,val.toStyledString().c_str(),strlen(val.toStyledString().c_str()),0);


	//接受服务器消息（判断success/error）
	//注册时还是由客户端的主线程来接受服务器发来的消息，在客户与客户之间通信时由子接受服务器的消息为了避免对方客户长时间不发送消息。
	//注册时如果服务器端没有发过来确认消息，客户端还是干不了其它事，所以注册时就让客户端主线程等待接受服务器给的回应，根据回应的消息才能决定接下来能不能干其它事。
	//注册时不许要启动子线程，因为服务器会很快直接回复注册的情况。所以就在主线程中直接接受服务器的消息。
	//服务器回复客户端的注册成功与否不许要将数据打包为Json包，直接通过 ok/err 来判断注册成功与否即可。
	char buff[128] = {0};
	if(-1 == recv(sockfd,buff,127,0))//recv
	{
		perror("recv error:");
		return ;
	}

	//注册失败
	if(strcmp(buff,"register fail") == 0)
	{
		cout<<"该用户名已注册过，请重新选择用户名进行注册"<<endl;
	}
	else
	{
		cout<<"恭喜，注册成功"<<endl;
	}
	//如果注册成功程序自动结束。
}

void do_login(int sockfd)
{
	cout<<"login()"<<endl;
	//用户输入name pw
	string name;
	string pw;
cout<<"请输入用户名："<<endl;
	cin>>name;
	cout<<"请输入密码："<<endl;
	cin>>pw;

	//Json打包数据
	Json::Value val;
	val["reason_type"] = REASON_TYPE_LOGIN;//消息类型,把Json包发给服务器后，由服务器进行解析消息的类型进行消息相应的处理。
	val["name"] = name.c_str();
	val["pw"] = pw.c_str();
	

	//发送
	send(sockfd,val.toStyledString().c_str(),strlen(val.toStyledString().c_str()),0);
	

	//接受服务器回应（判断success/error）服务器只需要发送ok/error表示登录成功与否就可，不许要进行打Json包。
	//与服务器进行直接沟通的操作，不许要让子线程去接受数据，因为直接与服务器进行沟通的，服务器会立即发送给客户端回应消息，所以不会造成recv的阻塞。
	char buff[1024] = {0};
	if(-1 == recv(sockfd,buff,127,0))
	{
		perror("recv error:");
		return ;
	}
	//cout<<"recv buff is:"<<buff<<endl;
	//登录成功后，服务器会在自己的数据库中的上线列表中将该用户的信息加入。
	if(strcmp(buff,"login success") == 0)
	{
		cout<<"登录成功"<<endl;
		//启动一个线程（专门接受数据和打印数据）
		pthread_t id;
		pthread_create(&id,0,pthread_run,(void *)&sockfd);
		while(1)
		{
			cout<<"请输入选择服务：1. get list  2. talk to one  3.talk to group  4.exit"<<endl;
			int select;
			cin>>select;
			getchar();
			switch(select)
			{
				case 1: do_get_list(sockfd);while(flg==0){} flg=0;break;
				case 2: do_talk_to_one(sockfd,name);break;
				case 3: do_talk_to_group(sockfd,name);break;
				case 4: do_exit(sockfd); exit(0);break;
				default:
						{
							cout<<"输入有误，请重新输入"<<endl;break;
						}
			}
		}
	}
	else if(strcmp(buff,"login fail") == 0)
	{
		cout<<"用户名不存在或密码错误"<<endl;
	}
	else if(strcmp(buff,"login second") == 0)//说明该用户已经在另一个客户端登录过了，不能重复登录。   如果想和qq的实现一样当这个登录时把另一个挤下线也可以，可以通过服务器发现重复登录就到在线列表中找到已经登录成功的那个，通过clifd发给那个客户端一个信息，那边客户端接受到这个信息后产生信号将自己执行退出程序退出掉。
	{
		cout<<"该帐号已经在另一个客户端登录，不能重复登录，登录失败"<<endl;
	}
	else//说明服务器发送过来的是要给这个客户端的离线消息。
	{
		cout<<"登录成功"<<endl;
		//登陆成功后，先将接受到的离线消息打印出来，再创建子线程。
		//离线消息格式：youname#message$youname#message$youname#message............
		int i=0;
		while(buff[i]!='\0')
		{
			while(buff[i]!='#')
			{
				cout<<buff[i++];
			}
			i++;
			cout<<"对我说：";
			while(buff[i]!='$'&&buff[i]!='\0')
			{
				cout<<buff[i++];
			}
			if(buff[i]=='\0')
			{
				cout<<endl;
				break;
			}
			i++;
			cout<<endl;
		}
		//启动一个线程（专门接受数据和打印数据）
		pthread_t id;
		pthread_create(&id,0,pthread_run,(void *)&sockfd);
		while(1)
		{
			cout<<"请输入选择服务：1. get list  2. talk to one  3.talk to group  4.exit"<<endl;
			int select;
			cin>>select;
			getchar();
			switch(select)
			{
				case 1: do_get_list(sockfd);while(flg==0){}flg=0;break;
				case 2: do_talk_to_one(sockfd,name);break;
				case 3: do_talk_to_group(sockfd,name);break;
				case 4: do_exit(sockfd); exit(0);break;
				default:
						{
							cout<<"输入有误，请重新输入"<<endl;break;
						}
			}
		}
	}
}

//只需要把请求的Json数据包做好，发送给服务器端就好，登录成功后，接受服务器回复的数据都是由最初启动的客户端子线程来做。
//客户端发送给服务器端的消息都是打包好的Json包。
//获取在线好友列表函数。
void do_get_list(int sockfd)
{	
	Json::Value val;
	val["reason_type"] = REASON_TYPE_LIST;//消息类型,把Json包发给服务器后，由服务器进行解析消息的类型进行消息相应的处理。

	//发送
	send(sockfd,val.toStyledString().c_str(),strlen(val.toStyledString().c_str()),0);
}

//把要给那个客户端发送的数据打包好交给服务器
void do_talk_to_one(int sockfd,string name)//name 代表自己的name，让服务器那边去解析
{
	char tmp[2048] = {0};
	string youname;
	string message;
	cout<<"请输入对方姓名"<<endl;
	fgets(tmp,2047,stdin);
	tmp[strlen(tmp)-1] = '\0';
	youname = tmp;
	cout<<"请输入你要发送的数据"<<endl;
	fgets(tmp,2047,stdin);
	tmp[strlen(tmp)-1] = '\0';
	message = tmp;
	Json::Value val;
	val["reason_type"] = REASON_TYPE_TALK;//消息类型,把Json包发给服务器后，由服务器进行解析消息的类型进行消息相应的处理。
	val["youname"] = youname.c_str();
	val["message"] = message.c_str();
	val["name"] = name.c_str();
	//发送
	send(sockfd,val.toStyledString().c_str(),strlen(val.toStyledString().c_str()),0);
	//成功与否的消息服务器发给了子线程，一会在子线程中完善功能。
}

//把要群发的消息打包好交给服务器
void do_talk_to_group(int sockfd,string myname)
{
	string othername;
	char tmp[2048] = {0};
	cout<<"请输入所有发送人姓名，每个姓名之间用#进行分割:"<<endl;
	//cin>>othername;
	fgets(tmp,2047,stdin);
	tmp[strlen(tmp)-1] = '\0';
	othername = tmp;
	string message;
	cout<<"请输入要群发的消息:"<<endl;
	//cin>>message;
	fgets(tmp,2047,stdin);
	tmp[strlen(tmp)-1] = '\0';
	message = tmp;
	Json::Value val;
	val["reason_type"] = REASON_TYPE_GROUP;//消息类型,把Json包发给服务器后，由服务器进行解析消息的类型进行消息相应的处理。
	val["othername"] = othername.c_str();////////////Json中不一定支持key值是vector类型数据，若不支持再修改。//////若不支持就调用私发消息的函数，对每个用户单独进行发送。
	val["name"] = myname.c_str();
	val["message"] = message.c_str();
	//发送
	send(sockfd,val.toStyledString().c_str(),strlen(val.toStyledString().c_str()),0);
	//成功与否的消息服务器发给了子线程，一会在子线程中完善功能。

}
//下线，将下线消息交给服务器，服务器在自己数据库中的上线列表中将该用户消息清除。
void do_exit(int sockfd)
{
	Json::Value val;
	val["reason_type"] = REASON_TYPE_EXIT;//消息类型,把Json包发给服务器后，由服务器进行解析消息的类型进行消息相应的处理。
	send(sockfd,val.toStyledString().c_str(),strlen(val.toStyledString().c_str()),0);
    cout<<"GoodBye"<<endl;
	//pid_t clipid = getpid(); 
	//kill(clipid,SIGKILL);
}
