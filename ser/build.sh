#!/bin/bash

#g++ -o server *.cpp -lpthread -L /usr/lib64/mysql -lmysqlclient -levent -ljsoncpp
cd build
cmake3 ..
make -j
