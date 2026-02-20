TARGET			:=	libvk.a
CC				:=	c++
BASE_CPPFLAGS	:=	-std=c++20 -Wall -Wextra -Werror
RELEASE_FLAGS	:=	-DNDEBUG -flto -O3 -march=native -fno-math-errno
DEBUG_FLAGS		:=	-g -fsanitize=address
CPPFLAGS		:=	$(BASE_CPPFLAGS)

INCLUDE			:=	-I. \
					-I/opt/homebrew/include \
					-I../vectors \
					-I../../include \
					-I/usr/local/include

SOURCES			:=	$(shell find . -type f -name '*.cpp')

BUILD_DIR	:=  build
OBJ_DIR		:=	$(BUILD_DIR)/obj
DEPS_DIR	:=  $(BUILD_DIR)/deps
OBJECTS		:=	$(addprefix $(OBJ_DIR)/,$(notdir $(SOURCES:%.cpp=%.o)))
DEPS		:=	$(addprefix $(DEPS_DIR)/,$(notdir $(SOURCES:.cpp=.d)))


all: $(TARGET)

release: CPPFLAGS = $(BASE_CPPFLAGS) $(RELEASE_FLAGS)
release: all

debug: CPPFLAGS = $(BASE_CPPFLAGS) $(DEBUG_FLAGS)
debug: all

$(OBJ_DIR) $(DEPS_DIR):
	mkdir -p $@

$(TARGET): $(OBJ_DIR) $(DEPS_DIR) $(OBJECTS)
	ar -rcs $(BUILD_DIR)/$(TARGET) $(OBJECTS)

$(OBJ_DIR)/%.o: %.cpp
	$(CC) $(CPPFLAGS) $(INCLUDE) -MMD -MF $(DEPS_DIR)/$*.d -c $< -o $@

-include $(DEPS)

clean:
	rm -rf $(OBJ_DIR)
	rm -rf $(DEPS_DIR)

fclean:
	rm -rf $(BUILD_DIR)

re: fclean all

.PHONY: all release debug clean fclean re