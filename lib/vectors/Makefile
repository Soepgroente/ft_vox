CC			:= c++

BASE_CPPFLAGS	:=	-std=c++20 -Wall -Wextra -Werror
RELEASE_FLAGS	:=	-DNDEBUG -flto -O3 -march=native -fno-math-errno
DEBUG_FLAGS		:=	-g -fsanitize=address
CPPFLAGS		:=	$(BASE_CPPFLAGS)
SRC_DIR			:=	source

INCLUDES	:=	-Iinclude \
				-I/opt/homebrew/include \
				-I./vulkan


TEST_EXEC	:=	testmath

SRCS		:=	$(shell find $(SRC_DIR) -maxdepth 1 -type f -name '*.cpp')
TEST_DIR	:=	$(SRC_DIR)/tests
TEST_SRCS	:=	$(shell find $(TEST_DIR) -type f -name '*.cpp')

BUILD_DIR	:=  build
OBJ_DIR		:=	$(BUILD_DIR)/obj
DEPS_DIR	:=  $(BUILD_DIR)/deps
TARGET		:=	$(BUILD_DIR)/libvectors.a

OBJS		:=	$(addprefix $(OBJ_DIR)/,$(notdir $(SRCS:%.cpp=%.o)))
TEST_OBJS	:=	$(addprefix $(OBJ_DIR)/,$(notdir $(TEST_SRCS:%.cpp=%.o)))
DEPS		:=	$(addprefix $(DEPS_DIR)/,$(notdir $(SRCS:.cpp=.d)))

all: $(TARGET)

test: $(TEST_EXEC)

runtest: $(TEST_EXEC)
	./$(TEST_EXEC)

release: CPPFLAGS = $(BASE_CPPFLAGS) $(RELEASE_FLAGS)
release: all

debug: CPPFLAGS = $(BASE_CPPFLAGS) $(DEBUG_FLAGS)
debug: all

$(OBJ_DIR) $(DEPS_DIR):
	mkdir -p $@

$(TEST_EXEC): $(TARGET) $(TEST_OBJS) $(OBJS)
	$(CC) $(CPPFLAGS) $(INCLUDES) -I$(TEST_DIR) -o $(TEST_EXEC) $(TEST_OBJS) $(OBJS) $(TARGET)

$(TARGET): $(OBJ_DIR) $(DEPS_DIR) $(OBJS)
	ar -rcs $@ $(OBJS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CC) $(CPPFLAGS) $(INCLUDES) -MMD -MF $(DEPS_DIR)/$*.d -c $< -o $@

$(OBJ_DIR)/%.o: $(TEST_DIR)/%.cpp
	$(CC) $(CPPFLAGS) $(INCLUDES) -I$(TEST_DIR) -MMD -MF $(DEPS_DIR)/$*.d -c $< -o $@

-include $(DEPS)

clean:
	rm -rf $(OBJ_DIR)
	rm -rf $(DEPS_DIR)

fclean:
	rm -rf $(BUILD_DIR)
	rm -f $(TEST_EXEC)

re: fclean all

.PHONY: all test runtest release debug clean fclean re