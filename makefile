.PHONY : clean all
# 构建需要的变量
CC = g++
D_SRC = src
D_TMP = tmp
D_INC = ext/include:$(D_SRC)
D_LIB = ext/lib
CXXFLAGS = -std=c++17
LDFLAGS = 
VPATH = $(D_INC)
LIB = mingw32 SDL2main imgui SDL2 opengl32

GL_SOURCES = $(wildcard $(D_SRC)/impl/*.cpp)
DEMO_SOURCES = $(wildcard $(D_SRC)/demo/*.cpp)
SOURCES += $(GL_SOURCES)
SOURCES += $(DEMO_SOURCES)

TARGETS = $(basename $(notdir $(DEMO_SOURCES)))

GL_OBJECTS = $(addprefix $(D_TMP)/,$(GL_SOURCES:%.cpp=%.o))
OBJECTS = $(addprefix $(D_TMP)/, $(SOURCES:%.cpp=%.o))

# 构建目标
all : $(TARGETS)

$(TARGETS): $(OBJECTS)
	$(CC) $(GL_OBJECTS) $(filter %$@.o, $^) -L$(subst :, -L,$(D_LIB)) $(LIB:%=-l%) $(LDFLAGS) -o $@

$(OBJECTS): $(SOURCES)
	@mkd $(dir $@)
	$(CC) -I$(subst :, -I,$(D_INC)) $(CXXFLAGS) -MMD -c $(subst $(D_TMP)/,,$(@:%.o=%.cpp)) -o $@


-include $(OBJECTS:%.o=%.d)


# 其它指令
clean :
	@clean