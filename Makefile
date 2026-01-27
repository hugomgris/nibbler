# -=-=-=-=-    COLOURS -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- #

DEF_COLOR   = \033[0;39m
YELLOW      = \033[0;93m
CYAN        = \033[0;96m
GREEN       = \033[0;92m
BLUE        = \033[0;94m
RED         = \033[0;91m

# -=-=-=-=-    NAMES -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- #

NAME			:= nibbler
SDL_LIB_NAME    := nibbler_sdl.so
RAYLIB_LIB_NAME  := nibbler_raylib.so
NCURSES_LIB_NAME := nibbler_ncurses.so

# -=-=-=-=-    DIRECTORIES -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= #

SRCDIR          := srcs
OBJDIR          := .obj
DEPDIR          := .dep
LIB_OBJDIR      := $(OBJDIR)/libs  # Add this line
INCDIR          := incs

# -=-=-=-=-    MAIN PROGRAM FILES -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- #

SRC             := main.cpp LibraryManager.cpp GameManager.cpp Snake.cpp Food.cpp Utils.cpp
SRCS            := $(addprefix $(SRCDIR)/, $(SRC))
OBJS            := $(addprefix $(OBJDIR)/, $(SRC:.cpp=.o))
DEPS            := $(addprefix $(DEPDIR)/, $(SRC:.cpp=.d))

INCLUDES        := -I$(INCDIR)

# -=-=-=-=-    FLAGS -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- #

CC              := c++
CFLAGS          := -Wall -Wextra -Werror -std=c++20 -g3 -O0 $(INCLUDES) #-fsanitize=address
LIB_CFLAGS      := -Wall -Wextra -Werror -std=c++20 -g3 -O0 -fPIC $(INCLUDES)
DEPFLAGS        := -MMD -MP
LDFLAGS         := -ldl -L$(PWD)/libs/ncurses/lib -lncursesw -Wl,-rpath,$(PWD)/libs/ncurses/lib

# -=-=-=-=-    EXTERNAL LIBRARIES -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- #

LIB_DIR         := libs
SDL_DIR         := $(LIB_DIR)/SDL2
RAYLIB_DIR      := $(LIB_DIR)/raylib
NCURSES_DIR     := $(LIB_DIR)/ncurses

SDL_REPO        := https://github.com/libsdl-org/SDL.git
RAYLIB_REPO     := https://github.com/raysan5/raylib.git
NCURSES_URL     := https://invisible-mirror.net/archives/ncurses/ncurses-6.4.tar.gz

# -=-=-=-=-    GRAPHIC LIBRARY SOURCE FILES -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- #

GFX_DIR          := srcs/graphics

SDL_SRC          := SDLGraphic.cpp
RAYLIB_SRC       := RaylibGraphic.cpp
NCURSES_SRC      := NCursesGraphic.cpp

SDL_OBJS         := .obj/libs/SDLGraphic.o
RAYLIB_OBJS      := .obj/libs/RaylibGraphic.o
NCURSES_OBJS     := .obj/libs/NCursesGraphic.o

GAME_OBJS        := $(OBJDIR)/Snake.o $(OBJDIR)/Food.o $(OBJDIR)/GameManager.o $(OBJDIR)/Utils.o 

# -=-=-=-=-    FLAGS FOR EACH LIBRARY -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- #

SDL_CFLAGS       := $(LIB_CFLAGS) -I$(SDL_DIR)/include
RAYLIB_CFLAGS    := $(LIB_CFLAGS) -I$(RAYLIB_DIR)/src -Wno-missing-field-initializers
NCURSES_CFLAGS   := $(LIB_CFLAGS) -I$(NCURSES_DIR)/include -I$(NCURSES_DIR)/include/ncursesw

SDL_LDFLAGS      := -L$(SDL_DIR)/build -lSDL2-2.0 -Wl,-rpath,$(SDL_DIR)/build
RAYLIB_LDFLAGS   := -L$(RAYLIB_DIR)/src -lraylib -lm -lpthread -ldl -lrt -lX11
NCURSES_LDFLAGS  := -L$(NCURSES_DIR)/lib -lncursesw -Wl,-rpath,$(NCURSES_DIR)/lib

# -=-=-=-=-    TARGETS -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- #

all: check_libs directories $(SDL_LIB_NAME) $(RAYLIB_LIB_NAME) $(NCURSES_LIB_NAME) $(NAME)

check_libs:
	@if [ ! -f "$(SDL_DIR)/CMakeLists.txt" ]; then \
		echo "$(YELLOW)SDL2 not found. Cloning...$(DEF_COLOR)"; \
		mkdir -p $(LIB_DIR); \
		git clone --depth 1 --branch release-2.28.x $(SDL_REPO) $(SDL_DIR); \
		cd $(SDL_DIR) && mkdir -p build && cd build && cmake .. && make -j4; \
		echo "$(GREEN)SDL2 built successfully$(DEF_COLOR)"; \
	fi
	@if [ ! -f "$(RAYLIB_DIR)/src/raylib.h" ]; then \
		echo "$(YELLOW)Raylib not found. Cloning...$(DEF_COLOR)"; \
		mkdir -p $(LIB_DIR); \
		git clone --depth 1 $(RAYLIB_REPO) $(RAYLIB_DIR); \
		cd $(RAYLIB_DIR)/src && make -j4; \
		echo "$(GREEN)Raylib built successfully$(DEF_COLOR)"; \
	fi
	@if [ ! -f "$(NCURSES_DIR)/lib/libncursesw.so" ]; then \
		echo "$(YELLOW)NCurses not found. Downloading and building...$(DEF_COLOR)"; \
		mkdir -p $(LIB_DIR); \
		cd $(LIB_DIR) && \
		curl -L -o ncurses.tar.gz $(NCURSES_URL) && \
		tar xzf ncurses.tar.gz && \
		mv ncurses-6.4 ncurses-src && \
		cd ncurses-src && \
		./configure --prefix=$(PWD)/$(NCURSES_DIR) \
			--enable-widec \
			--with-shared \
			--without-debug \
			--enable-pc-files \
			--with-pkg-config-libdir=$(PWD)/$(NCURSES_DIR)/lib/pkgconfig && \
		make -j4 && \
		make install && \
		cd .. && \
		rm -rf ncurses-src ncurses.tar.gz; \
		echo "$(GREEN)NCurses built successfully$(DEF_COLOR)"; \
	fi
	@echo "$(GREEN)All libraries ready$(DEF_COLOR)"

$(SDL_LIB_NAME): $(SDL_OBJS) $(GAME_OBJS)
	$(CC) -shared -o $@ $^ $(SDL_LDFLAGS)
	@echo "$(GREEN)Built $(SDL_LIB_NAME)$(DEF_COLOR)"

$(RAYLIB_LIB_NAME): $(RAYLIB_OBJS) $(GAME_OBJS)
	$(CC) -shared -o $@ $^ $(RAYLIB_LDFLAGS)
	@echo "$(GREEN)Built $(RAYLIB_LIB_NAME)$(DEF_COLOR)"

$(NCURSES_LIB_NAME): $(NCURSES_OBJS) $(GAME_OBJS)
	$(CC) -shared -o $@ $^ $(NCURSES_LDFLAGS)
	@echo "$(GREEN)Built $(NCURSES_LIB_NAME)$(DEF_COLOR)"

# SDL object file compilation
.obj/libs/SDLGraphic.o: $(GFX_DIR)/SDLGraphic.cpp Makefile
	@mkdir -p .obj/libs
	@mkdir -p .dep/libs
	$(CC) $(SDL_CFLAGS) $(DEPFLAGS) -c $< -o $@ -MF .dep/libs/SDLGraphic.d

# Raylib object file compilation
.obj/libs/RaylibGraphic.o: $(GFX_DIR)/RaylibGraphic.cpp Makefile
	@mkdir -p .obj/libs
	@mkdir -p .dep/libs
	$(CC) $(RAYLIB_CFLAGS) $(DEPFLAGS) -c $< -o $@ -MF .dep/libs/RaylibGraphic.d

# NCurses object file compilation
.obj/libs/NCursesGraphic.o: $(GFX_DIR)/NCursesGraphic.cpp Makefile
	@mkdir -p .obj/libs
	@mkdir -p .dep/libs
	$(CC) $(NCURSES_CFLAGS) $(DEPFLAGS) -c $< -o $@ -MF .dep/libs/NCursesGraphic.d

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp Makefile
	@mkdir -p $(@D)
	@mkdir -p $(DEPDIR)/$(*D)
	$(CC) $(LIB_CFLAGS) $(DEPFLAGS) -c $< -o $@ -MF $(DEPDIR)/$*.d

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(NAME) $(LDFLAGS)
	@echo "$(GREEN)Built $(NAME)$(DEF_COLOR)"

-include $(DEPS)
-include $(DEPDIR)/libs/*.d

directories:
	@mkdir -p $(OBJDIR)
	@mkdir -p $(LIB_OBJDIR)
	@mkdir -p $(DEPDIR)
	@mkdir -p $(DEPDIR)/libs

game: re
	./nibbler 30 30

clean:
	@/bin/rm -fr $(OBJDIR) $(DEPDIR)
	@echo "$(RED)Objects removed$(DEF_COLOR)"

fclean: clean
	@/bin/rm -f $(NAME) $(SDL_LIB_NAME) $(RAYLIB_LIB_NAME) $(NCURSES_LIB_NAME)
	@/bin/rm -fr $(SDL_DIR) $(RAYLIB_DIR) $(NCURSES_DIR)
	@/bin/rm -fr checks/valgrind-unified.txt checks/valgrind-ncurses-out.txt checks/valgrind-sdl-out.txt checks/valgrind-raylib-out.txt
	@echo "$(RED)Cleaned all binaries and external libraries$(DEF_COLOR)"

re: fclean all

.PHONY: all clean fclean re directories check_libs
