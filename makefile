.PHONY : clean all

# 构建需要的变量
CC = g++
SOURCE = $(wildcard *.cpp)
OBJECTS = ${SOURCE:%.cpp=%.o}
INCLUDE_DIR = ext/SDL2/include
LIB_DIR = ext/SDL2/lib
CFLAG = -MMD -std=c++17 -g
LIB = mingw32 SDL2main SDL2
VPATH = $(INCLUDE_DIR)
EXEC = razer

# 构建目标


SDL2demo: $(OBJECTS)
	$(CC) $^ -L$(LIB_DIR) $(LIB:%=-l%) -o $(EXEC)

$(OBJECTS) : %.o: %.cpp
	$(CC) -I$(INCLUDE_DIR) $(CFLAG) -c $< -o $@

-include *.d


# 其它指令
clean :
	-del *.o *.d *.exe