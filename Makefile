# -=-=-=-=-    COLOURS -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- #

DEF_COLOR   = \033[0;39m
YELLOW      = \033[0;93m
CYAN        = \033[0;96m
GREEN       = \033[0;92m
BLUE        = \033[0;94m
RED         = \033[0;91m

# -=-=-=-=-    NAME -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= #

NAME			:= nibbler
LIB1_NAME		:= testlib1.so
LIB2_NAME		:= testlib2.so
LIB3_NAME		:= testlib3.so

# -=-=-=-=-    FILES -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- #

SRC				=	main.cpp

LIB_SRC			=	testlib.cpp

SRCDIR			=	srcs
SRCS			=	$(addprefix $(SRCDIR)/, $(SRC))

LIB1_DIR		:= libs/testlib
LIB2_DIR		:= libs/testlib
LIB3_DIR		:= libs/testlib

LIB1_SRC		:= testlib1.cpp
LIB2_SRC		:= testlib2.cpp
LIB3_SRC		:= testlib3.cpp

OBJDIR			=	.obj
OBJS			=	$(addprefix $(OBJDIR)/, $(SRC:.cpp=.o))

LIB_OBJDIR		=	.obj/libs
LIB1_OBJS		:= $(addprefix $(LIB_OBJDIR)/, $(LIB1_SRC:.cpp=.o))
LIB2_OBJS		:= $(addprefix $(LIB_OBJDIR)/, $(LIB2_SRC:.cpp=.o))
LIB3_OBJS		:= $(addprefix $(LIB_OBJDIR)/, $(LIB3_SRC:.cpp=.o))

DEPDIR			=	.dep
DEPS			=	$(addprefix $(DEPDIR)/, $(SRC:.cpp=.d))
LIB_DEPS		=	$(addprefix $(DEPDIR)/libs/, $(LIB_SRC:.cpp=.d))

INC				=	./incs/
INCLUDES		=	-I./incs/

MAKE			=	Makefile

# -=-=-=-=-    FLAGS -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- #

CC				=	c++
CFLAGS			=	-Wall -Wextra -Werror -std=c++20 -g -O3 $(INCLUDES)
LIB_CFLAGS		=	-Wall -Wextra -Werror -std=c++20 -g -O3 -fPIC $(INCLUDES)
DEPFLAGS		=	-MMD -MP
LDFLAGS			=	-ldl

# -=-=-=-=-    TARGETS -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- #

all: directories $(LIB1_NAME) $(LIB2_NAME) $(LIB3_NAME) $(NAME)

directories:
	@mkdir -p $(OBJDIR)
	@mkdir -p $(LIB_OBJDIR)
	@mkdir -p $(DEPDIR)
	@mkdir -p $(DEPDIR)/libs

-include $(DEPS) $(LIB_DEPS)

$(LIB1_NAME): $(LIB1_OBJS)
	$(CC) -shared -o $@ $^

$(LIB2_NAME): $(LIB2_OBJS)
	$(CC) -shared -o $@ $^

$(LIB3_NAME): $(LIB3_OBJS)
	$(CC) -shared -o $@ $^

$(NAME): $(OBJS) $(LIB_NAME)
	$(CC) $(CFLAGS) $(OBJS) -o $(NAME) $(LDFLAGS)
	@echo "$(GREEN)Snakeboarding is not a crime$(DEF_COLOR)"

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp Makefile
	@mkdir -p $(@D)
	@mkdir -p $(DEPDIR)/$(*D)
	$(CC) $(CFLAGS) $(DEPFLAGS) -c $< -o $@ -MF $(DEPDIR)/$*.d

$(LIB_OBJDIR)/$(LIB1_SRC:.cpp=.o): $(LIB1_DIR)/$(LIB1_SRC) Makefile
	@mkdir -p $(@D)
	$(CC) $(LIB_CFLAGS) $(DEPFLAGS) -c $< -o $@

$(LIB_OBJDIR)/$(LIB2_SRC:.cpp=.o): $(LIB2_DIR)/$(LIB2_SRC) Makefile
	@mkdir -p $(@D)
	$(CC) $(LIB_CFLAGS) $(DEPFLAGS) -c $< -o $@

$(LIB_OBJDIR)/$(LIB3_SRC:.cpp=.o): $(LIB3_DIR)/$(LIB3_SRC) Makefile
	@mkdir -p $(@D)
	$(CC) $(LIB_CFLAGS) $(DEPFLAGS) -c $< -o $@

clean:
	@/bin/rm -fr $(OBJDIR) $(DEPDIR)
	@echo "$(RED)Objects removed$(DEF_COLOR)"

fclean: clean
	@/bin/rm -f $(NAME) $(LIB1_NAME) $(LIB2_NAME) $(LIB3_NAME)
	@echo "$(RED)Cleaned all binaries$(DEF_COLOR)"

re: fclean all

.PHONY: all clean fclean re directories
