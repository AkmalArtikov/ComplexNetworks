SOURCES		=main.cpp
EXECUTABLE	=prepare
BIN_FOLDER	=prepare


CC			=g++
LDFLAGS		=
INCLUDE		=../../libs
CPPFLAGS	=-Wall -Wextra -Werror -std=c++0x -I$(INCLUDE)
OBJECTS		=$(SOURCES:.cpp=.o)
BIN_ROOT	=../../bin


all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(FLAGS) $(BIN_ROOT)/$(BIN_FOLDER)/$(OBJECTS) -o $(BIN_ROOT)/$(BIN_FOLDER)/$@

.cpp.o:
	mkdir -p $(BIN_ROOT)/$(BIN_FOLDER)
	$(CC) $(CPPFLAGS) -c $< -o $(BIN_ROOT)/$(BIN_FOLDER)/$@
