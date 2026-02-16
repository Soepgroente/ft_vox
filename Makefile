TARGET			:=	ft_vox
CC				:=	c++
BASE_CPPFLAGS	:=	-std=c++20 -Wall -Wextra -Werror
RELEASE_FLAGS	:=	-DNDEBUG -flto -O3 -march=native -fno-math-errno
DEBUG_FLAGS		:=	-g -fsanitize=address

INCLUDE 		:=	-I ./include \
					-I /opt/homebrew/include \
					-I ./lib/vulkan \
					-I ./lib/vectors \
					-I /usr/local/include \

SRC_DIR		:= src
BUILD_DIR	:= build
OBJ_DIR		:= $(BUILD_DIR)/obj

LIB_DIR		:= lib
VECTORDIR	:= $(LIB_DIR)/vectors
VULKANDIR	:= $(LIB_DIR)/vulkan

LIBS		:= $(VECTORDIR)/vectors.a $(VULKANDIR)/vulkan.a

SOURCES		:=	Vox.cpp \
				InputHandler.cpp \
				KeyboardInput.cpp \
				main.cpp \
				MouseInput.cpp \
				utils.cpp \

OBJECTS		:=	$(addprefix $(OBJ_DIR)/,$(notdir $(SOURCES:%.cpp=%.o)))
UNAME_S		:=	$(shell uname -s)

SHADERS_DIR	:=	shaders
SHADERS_SRC	:=	basic.vert \
				basic.frag \

GLSLC				:= $(shell which glslc)
SHADERS_COMPILED	:= $(addprefix $(BUILD_DIR)/,$(addsuffix .spv,$(SHADERS_SRC)))

RPATH_DIR	:=	/usr/local/lib
LFLAGS		:=	-L/opt/homebrew/lib -lglfw -framework Cocoa -framework IOKit -framework OpenGL
LDFLAGS		:=	-lvulkan -lm -Wl,-rpath,$(RPATH_DIR)

ifeq ($(UNAME_S), Linux)
	INCLUDES += -isystem $(USER)/.capt/root/usr/include
	LFLAGS = -lglfw -lGL -lX11 -lpthread -lXrandr -lXi -ldl `pkg-config --static --libs glfw3`
	BASE_CPPFLAGS += `pkg-config --cflags glfw3`
endif

# source /opt/vulkan/current/setup-env.sh

CPPFLAGS = $(BASE_CPPFLAGS) $(RELEASE_FLAGS)

all: libs $(TARGET)

libs:
	$(MAKE) -C $(VECTORDIR)
	$(MAKE) -C $(VULKANDIR)

libs-debug: 
	$(MAKE) -C $(VECTORDIR) debug
	$(MAKE) -C $(VULKANDIR) debug

debug: CPPFLAGS = $(BASE_CPPFLAGS) $(DEBUG_FLAGS)
debug: libs-debug $(SHADERS_COMPILED) $(TARGET)

run: all
	./$(TARGET)

rundebug: debug
	./$(TARGET)

rerundebug: fclean rundebug

rerun: fclean run

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)
	mkdir -p $(OBJ_DIR)

$(TARGET): $(BUILD_DIR) $(SHADERS_COMPILED) $(OBJECTS)
	$(CC) $(CPPFLAGS) $(OBJECTS) $(INCLUDE) -o $(TARGET) $(LIBS) $(LDFLAGS) $(LFLAGS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CC) $(CPPFLAGS) $(INCLUDE) -c $< -o $@

$(BUILD_DIR)/%.spv: $(SHADERS_DIR)/%
	$(GLSLC) $< -o $@

clean:
	rm -rf $(BUILD_DIR)
	$(MAKE) -C $(VECTORDIR) clean
	$(MAKE) -C $(VULKANDIR) clean

fclean: clean
	rm -f $(TARGET)
	rm -f $(VECTORDIR)/vectors.a
	rm -f $(VULKANDIR)/vulkan.a

re: fclean all

.PHONY: all debug release clean run install-deps
