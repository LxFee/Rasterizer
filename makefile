.PHONY : clean all test

CC = g++
D_SRC = src
D_TMP = tmp
D_INC = ext/include $(D_SRC)/core $(D_SRC)/utils
D_LIB = ext/lib
CXXFLAGS = -std=c++17 -O3
LDFLAGS = 
LIB = mingw32 SDL2main imgui SDL2 opengl32

SOURCES = $(wildcard $(D_SRC)/utils/*.cpp)
CORE_SOURCES = $(wildcard $(D_SRC)/core/*.cpp)
DEMO_SOURCES = $(wildcard $(D_SRC)/demo/*.cpp)
TEST_SOURCES = $(wildcard $(D_SRC)/test/*.cpp)

OBJECTS = $(addprefix $(D_TMP)/,$(SOURCES:%.cpp=%.o))
CORE_OBJECTS = $(addprefix $(D_TMP)/,$(CORE_SOURCES:%.cpp=%.o))
DEMO_OBJECTS = $(addprefix $(D_TMP)/,$(DEMO_SOURCES:%.cpp=%.o))
TEST_OBJECTS = $(addprefix $(D_TMP)/,$(TEST_SOURCES:%.cpp=%.o))

TARGETS = $(basename $(notdir $(DEMO_SOURCES)))
TESTS = $(basename $(notdir $(TEST_SOURCES)))

# 构建目标
all : $(TARGETS)

test : $(TESTS)

$(TARGETS) : $(CORE_OBJECTS) $(OBJECTS) $(DEMO_OBJECTS)
	$(CC) $(CORE_OBJECTS) $(OBJECTS) $(filter %$@.o, $^) $(addprefix -L,$(D_LIB)) $(LIB:%=-l%) $(CXXFLAGS) $(LDFLAGS) -o $@

$(TESTS) : $(CORE_OBJECTS) $(TEST_OBJECTS)
	$(CC) $(CORE_OBJECTS) $(filter %$@.o, $^) $(addprefix -L,$(D_LIB)) $(LIB:%=-l%) $(CXXFLAGS) $(LDFLAGS) -o $@

$(OBJECTS) $(CORE_OBJECTS) $(DEMO_OBJECTS) $(TEST_OBJECTS) : $(D_TMP)/%.o : %.cpp
	@mkd $(dir $@)
	$(CC) $(addprefix -I,$(D_INC)) $(CXXFLAGS) -MMD -c $< -o $@

-include $(OBJECTS:%.o=%.d)
-include $(DEMO_OBJECTS:%.o=%.d)
-include $(TEST_OBJECTS:%.o=%.d)


# 其它指令
clean :
	@clean
