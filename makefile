.PHONY : clean all

CC = g++
D_SRC = src
D_TMP = tmp
D_INC = ext/include $(D_SRC)
D_LIB = ext/lib
CXXFLAGS = -std=c++17 -O3
LDFLAGS = 
LIB = mingw32 SDL2main imgui SDL2 opengl32

DEMO_SOURCES = $(wildcard $(D_SRC)/demo/*.cpp)

CORE_SOURCES = $(wildcard $(D_SRC)/core/impl/*.cpp)
# platform backend
CORE_SOURCES += $(wildcard $(D_SRC)/platforms/mingw32_sdl2_imgui/*.cpp)

SHADER_SOURCES = $(wildcard $(D_SRC)/shaders/*.cpp)
SHADER_SOURCES += $(wildcard $(D_SRC)/utils/impl/*.cpp)

SOURCES = $(DEMO_SOURCES) $(CORE_SOURCES) $(SHADER_SOURCES)

DEMO_OBJECTS = $(addprefix $(D_TMP)/,$(DEMO_SOURCES:%.cpp=%.o))
CORE_OBJECTS = $(addprefix $(D_TMP)/,$(CORE_SOURCES:%.cpp=%.o))
SHADER_OBJECTS = $(addprefix $(D_TMP)/,$(SHADER_SOURCES:%.cpp=%.o))
OBJECTS = $(addprefix $(D_TMP)/,$(SOURCES:%.cpp=%.o))

TARGETS = $(basename $(notdir $(DEMO_SOURCES)))

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
