NAME			:=	vulkan.a
CC				:=	c++
BASE_CPPFLAGS	:=	-std=c++20 -Wall -Wextra -Werror
RELEASE_FLAGS	:=	-DNDEBUG -flto -O3 -march=native -fno-math-errno
DEBUG_FLAGS		:=	-g -fsanitize=address

INCLUDE			:=	-I . \
					-I /opt/homebrew/include \
					-I ../vectors \
					-I ../../include \
					-I /usr/local/include \

SOURCES			:=	Camera.cpp \
					stb_image.cpp \
					VulkanBuffer.cpp \
					VulkanDescriptors.cpp \
					VulkanDevice.cpp \
					VulkanFrameInfo.cpp \
					VulkanModel.cpp \
					VulkanObject.cpp \
					VulkanObjFileParser.cpp \
					VulkanPipeline.cpp \
					VulkanRenderer.cpp \
					VulkanRenderSystem.cpp \
					VulkanSwapChain.cpp \
					VulkanTexture.cpp \
					VulkanWindow.cpp \

OBJ_DIR			:=	obj
OBJECTS			:=	$(addprefix $(OBJ_DIR)/,$(notdir $(SOURCES:%.cpp=%.o)))
CPPFLAGS		:=	$(BASE_CPPFLAGS) $(RELEASE_FLAGS)

all: $(NAME)

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

$(NAME): $(OBJ_DIR) $(OBJECTS)
	ar -rcs $(NAME) $(OBJECTS)

$(OBJ_DIR)/%.o: %.cpp
	$(CC) $(CPPFLAGS) $(INCLUDE) -c $< -o $@

debug: CPPFLAGS = $(BASE_CPPFLAGS) $(DEBUG_FLAGS)
debug: fclean $(NAME)

clean:
	rm -rf $(OBJ_DIR)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all debug clean fclean re