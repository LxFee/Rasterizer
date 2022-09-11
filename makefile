.PHONY : clean all test

CC = g++
D_SRC = src
D_TMP = tmp
D_INC = ext/include $(D_SRC)/core $(D_SRC)/shaders
D_LIB = ext/lib
CXXFLAGS = -std=c++17 -O3
LDFLAGS = 
LIB = mingw32 SDL2main imgui SDL2 opengl32

TEST_SOURCES = $(wildcard $(D_SRC)/test/*.cpp)
CORE_SOURCES = $(wildcard $(D_SRC)/core/*.cpp)
# platform backend
CORE_SOURCES += $(wildcard $(D_SRC)/platforms/mingw32_sdl2_imgui/*.cpp)
SHADER_SOURCES = $(wildcard $(D_SRC)/shaders/*.cpp)

SOURCES = $(TEST_SOURCES) $(CORE_SOURCES) $(SHADER_SOURCES)

TEST_OBJECTS = $(addprefix $(D_TMP)/,$(TEST_SOURCES:%.cpp=%.o))
CORE_OBJECTS = $(addprefix $(D_TMP)/,$(CORE_SOURCES:%.cpp=%.o))
SHADER_OBJECTS = $(addprefix $(D_TMP)/,$(SHADER_SOURCES:%.cpp=%.o))
OBJECTS = $(addprefix $(D_TMP)/,$(SOURCES:%.cpp=%.o))

TARGETS = $(basename $(notdir $(TEST_SOURCES)))

# 构建目标
all : $(TARGETS)

$(TARGETS) : $(OBJECTS)
	$(CC) $(CORE_OBJECTS) $(SHADER_OBJECTS) $(filter %$@.o, $^) $(addprefix -L,$(D_LIB)) $(LIB:%=-l%) $(CXXFLAGS) $(LDFLAGS) -o $@

$(OBJECTS) : $(D_TMP)/%.o : %.cpp
	@mkd $(dir $@)
	$(CC) $(addprefix -I,$(D_INC)) $(CXXFLAGS) -MMD -c $< -o $@

-include $(OBJECTS:%.o=%.d)

# 其它指令
clean :
	@clean
