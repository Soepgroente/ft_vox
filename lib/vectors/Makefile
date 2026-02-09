NAME		:= vectors.a
CC			:= c++

BASE_CPPFLAGS	:=	-Wall -Wextra -Werror -std=c++20 -fPIC
RELEASE_FLAGS	:=	-DNDEBUG -flto -O3 -march=native -fno-math-errno
DEBUG_FLAGS		:=	-g -fsanitize=address

INCLUDES	:=	-I/opt/homebrew/include \
				-I./vulkan \

SRCS		:=	Mat3.cpp \
				Mat4.cpp \
				Quat.cpp \
				Vec2.cpp \
				Vec3.cpp \
				Vec4.cpp \
				Vectors.cpp \


TEST_EXEC	:=	testmath

TEST_SRCS	:=	matrixTests.cpp \
				quaternionTests.cpp \
				testmain.cpp \
				vectorTests.cpp \
				utils.cpp \

TEST_DIR	:=	./tests
OBJDIR		:=	obj
OBJS		:=	$(addprefix $(OBJDIR)/,$(notdir $(SRCS:%.cpp=%.o)))
TEST_OBJS	:=	$(addprefix $(OBJDIR)/,$(notdir $(TEST_SRCS:%.cpp=%.o)))

CPPFLAGS	=	$(BASE_CPPFLAGS) $(RELEASE_FLAGS)

all: $(NAME)

test: $(TEST_EXEC)

runtest: $(TEST_EXEC)
	./$(TEST_EXEC)

debug: CPPFLAGS = $(BASE_CPPFLAGS) $(DEBUG_FLAGS)
debug: all

$(OBJDIR):
	mkdir -p $(OBJDIR)

$(TEST_EXEC): $(NAME) $(OBJDIR) $(TEST_OBJS) $(OBJS)
	$(CC) $(CPPFLAGS) $(INCLUDES) -I$(TEST_DIR) -o $(TEST_EXEC) $(TEST_OBJS) $(OBJS) $(NAME)

$(NAME): $(OBJDIR) $(OBJS)
	ar -rcs $(NAME) $(OBJS)

$(OBJDIR)/%.o: %.cpp
	$(CC) $(CPPFLAGS) $(INCLUDES) -c $< -o $@

$(OBJDIR)/%.o: $(TEST_DIR)/%.cpp
	$(CC) $(CPPFLAGS) $(INCLUDES) -I$(TEST_DIR) -c $< -o $@

clean:
	rm -rf $(OBJDIR)

fclean: clean
	rm -f $(NAME)
	rm -f $(TEST_EXEC)

re: fclean all

.PHONY: all clean fclean re