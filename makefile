.PHONY : clean all

# 构建需要的变量
CC = g++
SOURCE = $(wildcard *.cpp)
OBJECTS = ${SOURCE:%.cpp=%.o}
INCLUDE_DIR = ext/SDL2/include
LIB_DIR = ext/SDL2/lib
CFLAGS = -MMD -std=c++17
LDFLAGS = 
LIB = mingw32 SDL2main SDL2
VPATH = $(INCLUDE_DIR)
EXECUTABLE = Rasterizer

# 构建目标


$(EXECUTABLE): $(OBJECTS)
	$(CC) $^ -L$(LIB_DIR) $(LIB:%=-l%) $(LDFLAGS) -o $(EXECUTABLE)

$(OBJECTS) : %.o: %.cpp
	$(CC) -I$(INCLUDE_DIR) $(CFLAGS) -c $< -o $@

-include *.d


# 其它指令
clean :
	-del *.o *.d *.exe