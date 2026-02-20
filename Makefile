TARGET			:=	ft_vox
CC				:=	c++
BASE_CPPFLAGS	:=	-std=c++20 -Wall -Wextra -Werror
RELEASE_FLAGS	:=	-DNDEBUG -flto -O3 -march=native -fno-math-errno
DEBUG_FLAGS		:=	-g -fsanitize=address
CPPFLAGS		:=	$(BASE_CPPFLAGS)

INCLUDE 		:=	-I./include \
					-I/opt/homebrew/include \
					-I./lib/vulkan \
					-I./lib/vectors \
					-I/usr/local/include

SRC_DIR		:= src
BUILD_DIR	:= build
OBJ_DIR		:= $(BUILD_DIR)/obj
DEPS_DIR	:= $(BUILD_DIR)/deps

LIB_DIR		:= lib
VECTOR_DIR	:= $(LIB_DIR)/vectors
VULKAN_DIR	:= $(LIB_DIR)/vulkan

LIBS		:= $(VULKAN_DIR)/build/libvk.a $(VECTOR_DIR)/build/libvectors.a

SOURCES		:=	$(shell find $(SRC_DIR) -type f -name '*.cpp')
OBJECTS		:=	$(addprefix $(OBJ_DIR)/,$(notdir $(SOURCES:%.cpp=%.o)))
DEPS		:= $(patsubst $(SRC_DIR)%,$(DEPS_DIR)%,$(SOURCES:.cpp=.d))

UNAME_S		:=	$(shell uname -s)

SHADERS_DIR	:=	shaders
SHADERS_SRC	:=	basic.vert \
				basic.frag \

GLSLC				:= $(shell which glslc)
SHADERS_COMPILED	:= $(addprefix $(BUILD_DIR)/,$(addsuffix .spv,$(SHADERS_SRC)))

RPATH_DIR	:=	/usr/local/lib
LFLAGS		:=	-L/opt/homebrew/lib -lglfw -framework Cocoa -framework IOKit -framework OpenGL
LDFLAGS		:=	-lvulkan -Wl,-rpath,$(RPATH_DIR)

# source /opt/vulkan/current/setup-env.sh
ifeq ($(UNAME_S), Linux)
	INCLUDES += -isystem $(USER)/.capt/root/usr/include
	LFLAGS = -lGL -lX11 -lpthread -lXrandr -lXi $(shell pkg-config --static --libs glfw3)
endif

all: libs $(TARGET)

libs:
	$(MAKE) -C $(VECTOR_DIR)
	$(MAKE) -C $(VULKAN_DIR)

run: all
	./$(TARGET)

rerun: fclean run

release: CPPFLAGS = $(BASE_CPPFLAGS) $(RELEASE_FLAGS)
release: libs-release $(TARGET)

libs-release: 
	$(MAKE) -C $(VECTOR_DIR) release
	$(MAKE) -C $(VULKAN_DIR) release

run-release: release
	./$(TARGET)

rerun-release: fclean run-release

debug: CPPFLAGS = $(BASE_CPPFLAGS) $(DEBUG_FLAGS)
debug: libs-debug $(TARGET)

libs-debug: 
	$(MAKE) -C $(VECTOR_DIR) debug
	$(MAKE) -C $(VULKAN_DIR) debug

run-debug: debug
	./$(TARGET)

rerun-debug: fclean run-debug

$(BUILD_DIR) $(OBJ_DIR) $(DEPS_DIR):
	mkdir -p $@

$(TARGET): $(LIBS) $(OBJ_DIR) $(DEPS_DIR) $(SHADERS_COMPILED) $(OBJECTS)
	$(CC) $(CPPFLAGS) $(OBJECTS) $(INCLUDE) -o $(TARGET) $(LIBS) $(LDFLAGS) $(LFLAGS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CC) $(CPPFLAGS) $(INCLUDE) -MMD -MF $(DEPS_DIR)/$*.d -c $< -o $@

$(BUILD_DIR)/%.spv: $(SHADERS_DIR)/%
	$(GLSLC) $< -o $@

-include $(DEPS)

clean:
	rm -rf $(BUILD_DIR)
	$(MAKE) -C $(VECTOR_DIR) clean
	$(MAKE) -C $(VULKAN_DIR) clean

fclean:
	rm -rf $(BUILD_DIR)
	rm -rf $(TARGET)
	$(MAKE) -C $(VECTOR_DIR) fclean
	$(MAKE) -C $(VULKAN_DIR) fclean

re: fclean all

.PHONY: all libs run rerun release libs-release run-release rerun-release debug libs-debug run-debug rerun-debug clean fclean re
