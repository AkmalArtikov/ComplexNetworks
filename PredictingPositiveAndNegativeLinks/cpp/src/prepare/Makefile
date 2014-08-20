INCLUDE=../../libs
CPPFLAGS=-Wall -Wextra -Werror -std=c++0x -I$(INCLUDE)
LDFLAGS=
BIN_ROOT=../../bin
BIN_FOLDER=prepare

prepare: prepare.o
	mkdir -p $(BIN_ROOT)/$(BIN_FOLDER)
	g++ $(LDFLAGS) -o $(BIN_ROOT)/$(BIN_FOLDER)/prepare prepare.o

prepare.o: prepare.cpp
	g++ $(CPPFLAGS) -c prepare.cpp
