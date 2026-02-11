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
AUDIO_LIB_NAME   := nibbler_sdl_mix.so

# -=-=-=-=-    DIRECTORIES -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= #

SRCDIR          := srcs
OBJDIR          := .obj
DEPDIR          := .dep
LIB_OBJDIR      := $(OBJDIR)/libs  # Add this line
INCDIR          := incs

# -=-=-=-=-    MAIN PROGRAM FILES -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- #

SRC             := main.cpp LibraryManager.cpp GameManager.cpp Snake.cpp Food.cpp Utils.cpp
AI_SRC          := AI/AIConfig.cpp AI/FloodFill.cpp AI/Pathfinder.cpp AI/SnakeAI.cpp AI/GridHelper.cpp
ALL_SRC         := $(SRC) $(AI_SRC)

SRCS            := $(addprefix $(SRCDIR)/, $(ALL_SRC))
OBJS            := $(addprefix $(OBJDIR)/, $(SRC:.cpp=.o)) $(addprefix $(OBJDIR)/, $(AI_SRC:.cpp=.o))
DEPS            := $(addprefix $(DEPDIR)/, $(SRC:.cpp=.d)) $(addprefix $(DEPDIR)/, $(AI_SRC:.cpp=.d))

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
SDL_TTF_DIR     := $(LIB_DIR)/SDL_ttf
SDL_MIXER_DIR   := $(LIB_DIR)/SDL_mixer
RAYLIB_DIR      := $(LIB_DIR)/raylib
NCURSES_DIR     := $(LIB_DIR)/ncurses

SDL_REPO        := https://github.com/libsdl-org/SDL.git
SDL_TTF_REPO    := https://github.com/libsdl-org/SDL_ttf.git
SDL_MIXER_REPO  := https://github.com/libsdl-org/SDL_mixer.git
RAYLIB_REPO     := https://github.com/raysan5/raylib.git
NCURSES_URL     := https://invisible-mirror.net/archives/ncurses/ncurses-6.4.tar.gz

# -=-=-=-=-    GRAPHIC LIBRARY SOURCE FILES -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- #

GFX_DIR          := srcs/graphics
AUDIO_DIR        := srcs/Audio

SDL_SRC          := SDLGraphic.cpp SDLParticleSystem.cpp SDLTextRenderer.cpp SDLTitleHandler.cpp
RAYLIB_SRC       := RaylibGraphic.cpp RaylibTitleHandler.cpp RaylibTextRenderer.cpp
NCURSES_SRC      := NCursesGraphic.cpp
AUDIO_SRC        := SDLMIXAudio.cpp

SDL_OBJS         := .obj/libs/SDLGraphic.o .obj/libs/SDLParticleSystem.o .obj/libs/SDLTextRenderer.o .obj/libs/SDLTitleHandler.o
RAYLIB_OBJS      := .obj/libs/RaylibGraphic.o .obj/libs/RaylibTitleHandler.o .obj/libs/RaylibTextRenderer.o
NCURSES_OBJS     := .obj/libs/NCursesGraphic.o
AUDIO_OBJS       := .obj/libs/SDLMIXAudio.o

AI_OBJS          := $(OBJDIR)/AI/AIConfig.o $(OBJDIR)/AI/FloodFill.o $(OBJDIR)/AI/Pathfinder.o $(OBJDIR)/AI/SnakeAI.o $(OBJDIR)/AI/GridHelper.o
GAME_OBJS        := $(OBJDIR)/Snake.o $(OBJDIR)/Food.o $(OBJDIR)/GameManager.o $(OBJDIR)/Utils.o $(AI_OBJS) 

# -=-=-=-=-    FLAGS FOR EACH LIBRARY -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- #

SDL_CFLAGS       := $(LIB_CFLAGS) -I$(SDL_DIR)/include -I$(SDL_TTF_DIR)
RAYLIB_CFLAGS    := $(LIB_CFLAGS) -I$(RAYLIB_DIR)/src -Wno-missing-field-initializers
NCURSES_CFLAGS   := $(LIB_CFLAGS) -I$(NCURSES_DIR)/include -I$(NCURSES_DIR)/include/ncursesw
AUDIO_CFLAGS     := $(LIB_CFLAGS) -I$(SDL_MIXER_DIR)/include

SDL_LDFLAGS      := -L$(SDL_DIR)/build -lSDL2 -L$(SDL_TTF_DIR)/build -lSDL2_ttf -Wl,-rpath,$(SDL_DIR)/build -Wl,-rpath,$(SDL_TTF_DIR)/build
RAYLIB_LDFLAGS   := -L$(RAYLIB_DIR)/src -lraylib -lm -lpthread -ldl -lrt -lX11
NCURSES_LDFLAGS  := -L$(NCURSES_DIR)/lib -lncursesw -Wl,-rpath,$(NCURSES_DIR)/lib
AUDIO_LDFLAGS    := -L$(SDL_MIXER_DIR)/build -lSDL2_mixer -Wl,-rpath,$(SDL_MIXER_DIR)/build


# -=-=-=-=-    GOOGLE TEST -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- #

GTEST_DIR		:= $(LIB_DIR)/googletest
GTEST_REPO		:= https://github.com/google/googletest.git
GTEST_LIB		:= $(GTEST_DIR)/build/lib/libgtest.a
GTEST_MAIN_LIB	:= $(GTEST_DIR)/build/lib/libgtest_main.a
GTEST_INCLUDES	:= -I$(GTEST_DIR)/googletest/include

# -=-=-=-=-    TEST CONFIGURATION -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- #

TEST_DIR		:= tests
TEST_OBJDIR		:= .test_obj
TEST_DEPDIR		:= .test_dep
TEST_BINARY		:= run_tests

TEST_SRCS		:= $(wildcard $(TEST_DIR)/unit/*.cpp) \
					$(wildcard $(TEST_DIR)/integration/*.cpp)
TEST_OBJS		:= $(patsubst $(TEST_DIR)/%.cpp,$(TEST_OBJDIR)/%.o,$(TEST_SRCS))
TEST_DEPS		:= $(patsubst $(TEST_DIR)/%.cpp,$(TEST_DEPDIR)/%.d,$(TEST_SRCS))

TESTABLE_SRCS	:= $(filter-out $(SRCDIR)/main.cpp, $(SRCS))
TESTABLE_OBJS	:= $(patsubst $(SRCDIR)/%.cpp,$(OBJDIR)/%.o,$(TESTABLE_SRCS))

TEST_CFLAGS		:= $(CFLAGS) $(GTEST_INCLUDES)
TEST_LDFLAGS	:= $(LDFLAGS) -lpthread



# -=-=-=-=-    TARGETS -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- #

all: directories
	@$(MAKE) check_libs
	@$(MAKE) $(SDL_LIB_NAME) $(RAYLIB_LIB_NAME) $(NCURSES_LIB_NAME) $(AUDIO_LIB_NAME) $(NAME)

check_libs:
	@if [ ! -f "$(SDL_DIR)/build/libSDL2-2.0.so" ] && [ ! -f "$(SDL_DIR)/build/libSDL2.so" ]; then \
		echo "$(YELLOW)SDL2 shared library not found. Building...$(DEF_COLOR)"; \
		mkdir -p $(LIB_DIR); \
		if [ ! -f "$(SDL_DIR)/CMakeLists.txt" ]; then \
			git clone --depth 1 --branch release-2.28.x $(SDL_REPO) $(SDL_DIR); \
		fi; \
		cd $(SDL_DIR) && rm -rf build && mkdir -p build && cd build && \
		cmake -DCMAKE_BUILD_TYPE=Release \
			-DBUILD_SHARED_LIBS=ON .. && \
		make -j4; \
		if [ -f libSDL2-2.0.so ] && [ ! -f libSDL2.so ]; then \
			ln -sf libSDL2-2.0.so libSDL2.so; \
		fi; \
		echo "$(GREEN)SDL2 built successfully$(DEF_COLOR)"; \
	elif [ -f "$(SDL_DIR)/build/libSDL2.a" ]; then \
		echo "$(YELLOW)SDL2 static library found, rebuilding as shared...$(DEF_COLOR)"; \
		cd $(SDL_DIR) && rm -rf build && mkdir -p build && cd build && \
		cmake -DCMAKE_BUILD_TYPE=Release \
			-DBUILD_SHARED_LIBS=ON .. && \
		make -j4; \
		if [ -f libSDL2-2.0.so ] && [ ! -f libSDL2.so ]; then \
			ln -sf libSDL2-2.0.so libSDL2.so; \
		fi; \
		echo "$(GREEN)SDL2 rebuilt as shared library$(DEF_COLOR)"; \
	fi
	@if [ ! -f "$(SDL_TTF_DIR)/build/libSDL2_ttf-2.0.so" ] && [ ! -f "$(SDL_TTF_DIR)/build/libSDL2_ttf.so" ]; then \
		echo "$(YELLOW)SDL_ttf not found. Cloning and building...$(DEF_COLOR)"; \
		mkdir -p $(LIB_DIR); \
		if [ ! -f "$(SDL_TTF_DIR)/CMakeLists.txt" ]; then \
			git clone --depth 1 --branch release-2.22.x $(SDL_TTF_REPO) $(SDL_TTF_DIR); \
		fi; \
		if [ ! -d "$(SDL_TTF_DIR)/external/freetype/.git" ]; then \
			cd $(SDL_TTF_DIR)/external && ./download.sh; \
		fi; \
		cd $(SDL_TTF_DIR) && rm -rf build && mkdir -p build && cd build && \
		cmake -DSDL2_DIR=$(PWD)/$(SDL_DIR) \
			-DCMAKE_BUILD_TYPE=Release \
			-DBUILD_SHARED_LIBS=ON \
			-DSDL2TTF_VENDORED=ON .. && \
		make -j4; \
		if [ -f libSDL2_ttf-2.0.so ] && [ ! -f libSDL2_ttf.so ]; then \
			ln -sf libSDL2_ttf-2.0.so libSDL2_ttf.so; \
		fi; \
		echo "$(GREEN)SDL_ttf built successfully$(DEF_COLOR)"; \
	fi
	@if [ ! -f "$(SDL_MIXER_DIR)/CMakeLists.txt" ]; then \
		echo "$(YELLOW)SDL_mixer not found. Cloning...$(DEF_COLOR)"; \
		mkdir -p $(LIB_DIR); \
		git clone --depth 1 --branch release-2.8.x $(SDL_MIXER_REPO) $(SDL_MIXER_DIR); \
		cd $(SDL_MIXER_DIR) && mkdir -p build && cd build && \
		cmake -DSDL2_DIR=$(PWD)/$(SDL_DIR) \
			-DSDL2MIXER_OPUS=OFF \
			-DSDL2MIXER_FLAC=OFF \
			-DSDL2MIXER_MOD=OFF \
			-DSDL2MIXER_MIDI=OFF \
			-DSDL2MIXER_WAVPACK=OFF .. && \
		make -j4; \
		echo "$(GREEN)SDL_mixer built successfully$(DEF_COLOR)"; \
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

check_gtest:
	@if [ ! -f "$(GTEST_LIB)" ]; then \
		echo "$(YELLOW)Google Test not found. Cloning and building...$(DEF_COLOR)"; \
		mkdir -p $(LIB_DIR); \
		git clone --depth 1 --branch v1.14.0 $(GTEST_REPO) $(GTEST_DIR); \
		cd $(GTEST_DIR) && mkdir -p build && cd build && \
		cmake -DCMAKE_CXX_STANDARD=20 .. && \
		make -j4; \
		echo "$(GREEN)Google Test built successfully$(DEF_COLOR)"; \
	else \
		echo "$(GREEN)Google Test already built$(DEF_COLOR)"; \
	fi

$(SDL_LIB_NAME): $(SDL_OBJS) $(GAME_OBJS) | check_libs
	$(CC) -shared -o $@ $^ $(SDL_LDFLAGS)
	@echo "$(GREEN)Built $(SDL_LIB_NAME)$(DEF_COLOR)"

$(RAYLIB_LIB_NAME): $(RAYLIB_OBJS) $(GAME_OBJS) | check_libs
	$(CC) -shared -o $@ $^ $(RAYLIB_LDFLAGS)
	@echo "$(GREEN)Built $(RAYLIB_LIB_NAME)$(DEF_COLOR)"

$(NCURSES_LIB_NAME): $(NCURSES_OBJS) $(GAME_OBJS) | check_libs
	$(CC) -shared -o $@ $^ $(NCURSES_LDFLAGS)
	@echo "$(GREEN)Built $(NCURSES_LIB_NAME)$(DEF_COLOR)"

$(AUDIO_LIB_NAME): $(AUDIO_OBJS) | check_libs
	$(CC) -shared -o $@ $^ $(AUDIO_LDFLAGS)
	@echo "$(GREEN)Built $(AUDIO_LIB_NAME)$(DEF_COLOR)"

# SDL object file compilation
.obj/libs/SDLGraphic.o: $(GFX_DIR)/SDLGraphic.cpp Makefile | check_libs
	@mkdir -p .obj/libs
	@mkdir -p .dep/libs
	$(CC) $(SDL_CFLAGS) $(DEPFLAGS) -c $< -o $@ -MF .dep/libs/SDLGraphic.d

# ParticleSystem object file compilation (for SDL)
.obj/libs/SDLParticleSystem.o: $(GFX_DIR)/SDLParticleSystem.cpp Makefile | check_libs
	@mkdir -p .obj/libs
	@mkdir -p .dep/libs
	$(CC) $(SDL_CFLAGS) $(DEPFLAGS) -c $< -o $@ -MF .dep/libs/SDLParticleSystem.d

# TextRenderer object file compilation (for SDL)
.obj/libs/SDLTextRenderer.o: $(GFX_DIR)/SDLTextRenderer.cpp Makefile | check_libs
	@mkdir -p .obj/libs
	@mkdir -p .dep/libs
	$(CC) $(SDL_CFLAGS) $(DEPFLAGS) -c $< -o $@ -MF .dep/libs/SDLTextRenderer.d

# TitleHandler object file compilation (for SDL)
.obj/libs/SDLTitleHandler.o: $(GFX_DIR)/SDLTitleHandler.cpp Makefile | check_libs
	@mkdir -p .obj/libs
	@mkdir -p .dep/libs
	$(CC) $(SDL_CFLAGS) $(DEPFLAGS) -c $< -o $@ -MF .dep/libs/SDLTitleHandler.d

# Raylib object file compilation
.obj/libs/RaylibGraphic.o: $(GFX_DIR)/RaylibGraphic.cpp Makefile | check_libs
	@mkdir -p .obj/libs
	@mkdir -p .dep/libs
	$(CC) $(RAYLIB_CFLAGS) $(DEPFLAGS) -c $< -o $@ -MF .dep/libs/RaylibGraphic.d

# Raylib TitleHandler object file compilation
.obj/libs/RaylibTitleHandler.o: $(GFX_DIR)/RaylibTitleHandler.cpp Makefile | check_libs
	@mkdir -p .obj/libs
	@mkdir -p .dep/libs
	$(CC) $(RAYLIB_CFLAGS) $(DEPFLAGS) -c $< -o $@ -MF .dep/libs/RaylibTitleHandler.d

# Raylib TextRenderer object file compilation
.obj/libs/RaylibTextRenderer.o: $(GFX_DIR)/RaylibTextRenderer.cpp Makefile | check_libs
	@mkdir -p .obj/libs
	@mkdir -p .dep/libs
	$(CC) $(RAYLIB_CFLAGS) $(DEPFLAGS) -c $< -o $@ -MF .dep/libs/RaylibTextRenderer.d

# NCurses object file compilation
.obj/libs/NCursesGraphic.o: $(GFX_DIR)/NCursesGraphic.cpp Makefile | check_libs
	@mkdir -p .obj/libs
	@mkdir -p .dep/libs
	$(CC) $(NCURSES_CFLAGS) $(DEPFLAGS) -c $< -o $@ -MF .dep/libs/NCursesGraphic.d

# Audio object file compilation
.obj/libs/SDLMIXAudio.o: $(AUDIO_DIR)/SDLMIXAudio.cpp Makefile | check_libs
	@mkdir -p .obj/libs
	@mkdir -p .dep/libs
	$(CC) $(AUDIO_CFLAGS) $(DEPFLAGS) -c $< -o $@ -MF .dep/libs/SDLMIXAudio.d

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp Makefile
	@mkdir -p $(@D)
	@mkdir -p $(DEPDIR)/$(*D)
	$(CC) $(LIB_CFLAGS) $(DEPFLAGS) -c $< -o $@ -MF $(DEPDIR)/$*.d

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(NAME) $(LDFLAGS)
	@echo "$(GREEN)Built $(NAME)$(DEF_COLOR)"
	@echo "$(RED)Snakeboarding is not a crime!$(DEF_COLOR)"

-include $(DEPS)
-include $(DEPDIR)/libs/*.d

directories:
	@mkdir -p $(OBJDIR)
	@mkdir -p $(OBJDIR)/AI
	@mkdir -p $(LIB_OBJDIR)
	@mkdir -p $(DEPDIR)
	@mkdir -p $(DEPDIR)/AI
	@mkdir -p $(DEPDIR)/libs

game: re
	./nibbler 30 30

# -=-=-=-=-    TEST TARGETS -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- #

test: check_gtest $(TEST_BINARY)
	@echo "$(CYAN)Running tests...$(DEF_COLOR)"
	./$(TEST_BINARY)

$(TEST_BINARY): $(TESTABLE_OBJS) $(TEST_OBJS) $(GTEST_LIB) $(GTEST_MAIN_LIB)
	@echo "$(YELLOW)Linking test binary...$(DEF_COLOR)"
	$(CC) -o $@ $(TESTABLE_OBJS) $(TEST_OBJS) $(GTEST_LIB) $(GTEST_MAIN_LIB) $(TEST_LDFLAGS)
	@echo "$(GREEN)Test binary created: $(TEST_BINARY)$(DEF_COLOR)"

# Compile test object files
$(TEST_OBJDIR)/%.o: $(TEST_DIR)/%.cpp | $(TEST_OBJDIR) $(TEST_DEPDIR)
	@mkdir -p $(dir $@)
	@mkdir -p $(dir $(TEST_DEPDIR)/$*.d)
	$(CC) $(TEST_CFLAGS) -MMD -MF $(TEST_DEPDIR)/$*.d -c $< -o $@
	@echo "$(BLUE)Compiled test: $<$(DEF_COLOR)"

# Create test directories
$(TEST_OBJDIR):
	@mkdir -p $(TEST_OBJDIR)/unit $(TEST_OBJDIR)/integration

$(TEST_DEPDIR):
	@mkdir -p $(TEST_DEPDIR)/unit $(TEST_DEPDIR)/integration

# -=-=-=-=-    CLEANUP -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- #

clean:
	@/bin/rm -fr $(OBJDIR) $(DEPDIR) $(LIB_OBJDIR) $(TEST_OBJDIR) $(TEST_DEPDIR)
	@echo "$(RED)Object files removed$(DEF_COLOR)"

fclean: clean
	@/bin/rm -f $(NAME) $(SDL_LIB_NAME) $(RAYLIB_LIB_NAME) $(NCURSES_LIB_NAME) $(AUDIO_LIB_NAME) $(TEST_BINARY)
	@/bin/rm -fr $(SDL_DIR) $(SDL_TTF_DIR) $(SDL_MIXER_DIR) $(RAYLIB_DIR) $(NCURSES_DIR) $(GTEST_DIR)
	@/bin/rm -fr checks/valgrind-unified.txt checks/valgrind-ncurses-out.txt checks/valgrind-sdl-out.txt checks/valgrind-raylib-out.txt
	@echo "$(RED)Cleaned all binaries, external libraries and memory logs$(DEF_COLOR)"

re: fclean all

.PHONY: all clean fclean re directories check_libs test check_gtest game
