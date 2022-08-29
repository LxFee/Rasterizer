.PHONY : clean all

# 构建需要的变量
CC = g++
D_SRC = src
D_TMP = tmp
D_INC = ext/include $(D_SRC)
D_LIB = ext/lib
CXXFLAGS = -std=c++17 -O3
LDFLAGS = -fopenmp
LIB = mingw32 SDL2main imgui SDL2 opengl32

SOURCES = $(wildcard $(D_SRC)/impl/*.cpp)
DEMO_SOURCES = $(wildcard $(D_SRC)/demo/*.cpp)

ALL_SOURCES += $(SOURCES)
ALL_SOURCES += $(DEMO_SOURCES)

TARGETS = $(basename $(notdir $(DEMO_SOURCES)))

OBJECTS = $(addprefix $(D_TMP)/,$(SOURCES:%.cpp=%.o))
ALL_OBJECTS = $(addprefix $(D_TMP)/, $(ALL_SOURCES:%.cpp=%.o))

# 构建目标
all : $(TARGETS)

$(TARGETS): $(ALL_OBJECTS)
	$(CC) $(OBJECTS) $(filter %$@.o, $^) $(addprefix -L,$(D_LIB)) $(LIB:%=-l%) $(CXXFLAGS) $(LDFLAGS) -o $@

$(ALL_OBJECTS): $(D_TMP)/%.o : %.cpp
	@mkd $(dir $@)
	$(CC) $(addprefix -I,$(D_INC)) $(CXXFLAGS) -MMD -c $< -o $@


-include $(ALL_OBJECTS:%.o=%.d)


# 其它指令
clean :
	@clean
