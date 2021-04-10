# cmake 最低版本需求
cmake_minimum_required(VERSION 3.13)

# 工程名称
project(myProject)

SET(SRC_LIST control.cpp exit.cpp get_list.cpp login.cpp main.cpp mysql.cpp pthread.cpp register.cpp talk_to_group.cpp talk_to_one.cpp tcpserver.cpp)

#g++ -o server *.cpp -lpthread -L /usr/lib64/mysql -lmysqlclient -levent -ljsoncpp
link_directories("/usr/lib64/mysql")


# 编译源码生成目标
add_executable(server ${SRC_LIST})

target_link_libraries(server -lmysqlclient)
target_link_libraries(server -levent)
target_link_libraries(server -ljsoncpp)
target_link_libraries(server -lpthread)
