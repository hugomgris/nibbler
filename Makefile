# -=-=-=-=-    COLOURS -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- #

DEF_COLOR   = \033[0;39m
YELLOW      = \033[0;93m
CYAN        = \033[0;96m
GREEN       = \033[0;92m
BLUE        = \033[0;94m
RED         = \033[0;91m

# -=-=-=-=-    NAME -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= #

NAME			:= nibbler
LIB_NAME		:= testlib.so

# -=-=-=-=-    FILES -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- #

SRC				=	main.cpp

LIB_SRC			=	testlib.cpp

SRCDIR			=	srcs
SRCS			=	$(addprefix $(SRCDIR)/, $(SRC))

LIBDIR			=	libs/testlib
LIB_SRCS		=	$(addprefix $(LIBDIR)/, $(LIB_SRC))

OBJDIR			=	.obj
OBJS			=	$(addprefix $(OBJDIR)/, $(SRC:.cpp=.o))

LIB_OBJDIR		=	.obj/libs
LIB_OBJS		=	$(addprefix $(LIB_OBJDIR)/, $(LIB_SRC:.cpp=.o))

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

all: directories $(LIB_NAME) $(NAME)

directories:
	@mkdir -p $(OBJDIR)
	@mkdir -p $(LIB_OBJDIR)
	@mkdir -p $(DEPDIR)
	@mkdir -p $(DEPDIR)/libs

-include $(DEPS) $(LIB_DEPS)

$(LIB_NAME): $(LIB_OBJS)
	$(CC) -shared -o $@ $^

$(NAME): $(OBJS) $(LIB_NAME)
	$(CC) $(CFLAGS) $(OBJS) -o $(NAME) $(LDFLAGS)
	@echo "$(GREEN)Snakeboarding is not a crime$(DEF_COLOR)"

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp Makefile
	@mkdir -p $(@D)
	@mkdir -p $(DEPDIR)/$(*D)
	$(CC) $(CFLAGS) $(DEPFLAGS) -c $< -o $@ -MF $(DEPDIR)/$*.d

$(LIB_OBJDIR)/%.o: $(LIBDIR)/%.cpp Makefile
	@mkdir -p $(@D)
	@mkdir -p $(DEPDIR)/libs/$(*D)
	$(CC) $(LIB_CFLAGS) $(DEPFLAGS) -c $< -o $@ -MF $(DEPDIR)/libs/$*.d

clean:
	@/bin/rm -fr $(OBJDIR) $(DEPDIR)
	@echo "$(RED)Objects removed$(DEF_COLOR)"

fclean: clean
	@/bin/rm -f $(NAME) $(LIB_NAME)
	@echo "$(RED)Cleaned all binaries$(DEF_COLOR)"

re: fclean all

.PHONY: all clean fclean re directories
