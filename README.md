# simpleChat
端对端的简易chat项目


client采用autools工具生成Makefile
server采用cmake3工具生成Makefile


client和server都有build.sh脚本完成编译, start.sh完成启动进程





描述：
开发环境及第三方库：
        linux、c++、g++、gdb、vscode、libevent、jsconcpp、memcached、mysql
项目描述：
1、端对端文本通讯，注册并登录验证成功后可和好友进行即时通讯。
2、采用Reactor事件处理模式和高效的半同步/半异步的并发模式进行服务器的开发，通信部分采用libevent来实现数据的处理。使用MySQL对数据进行存储。使用memcached作为缓存存储热点数据提高访问数据速率
系统功能：注册、登陆、获取好友列表、私聊、广播、离线消息缓存、下线
