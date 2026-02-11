# ═══════════════════════════════════════════════════════════════════════════ #
#                              ROSARIO V2 MAKEFILE                              #
#                         Raylib-Unified Snake Game Build                       #
# ═══════════════════════════════════════════════════════════════════════════ #

# -=-=-=-=-    COLOURS -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-════=-=-=-=-=-=-=-=- #

DEF_COLOR   = \033[0;39m
YELLOW      = \033[0;93m
CYAN        = \033[0;96m
GREEN        = \033[0;92m
BLUE        = \033[0;94m
RED         = \033[0;91m

# -=-=-=-=-    NAMES -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- #

NAME                := nibbler

# -=-=-=-=-    DIRECTORIES -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- #

SRCDIR          := srcs
OBJDIR          := .obj
DEPDIR          := .dep
INCDIR          := incs

# -=-=-=-=-    SOURCE FILES -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- #

# Core game files
CORE_SRC        := main.cpp GameManager.cpp Snake.cpp Food.cpp Utils.cpp

# AI system files
AI_SRC          := AI/AIConfig.cpp AI/FloodFill.cpp AI/Pathfinder.cpp AI/SnakeAI.cpp AI/GridHelper.cpp

# Raylib graphics files
GRAPHICS_SRC    := graphics/RaylibGraphic.cpp graphics/RaylibTextRenderer.cpp graphics/RaylibTitleHandler.cpp

# SDL Particle system (to be ported)
PARTICLE_SRC    := graphics/SDLParticleSystem.cpp

# All sources combined
ALL_SRC         := $(CORE_SRC) $(AI_SRC) $(GRAPHICS_SRC) $(PARTICLE_SRC)

SRCS            := $(addprefix $(SRCDIR)/, $(ALL_SRC))
OBJS            := $(addprefix $(OBJDIR)/, $(ALL_SRC:.cpp=.o))
DEPS            := $(addprefix $(DEPDIR)/, $(ALL_SRC:.cpp=.d))

# -=-=-=-=-    INCLUDES -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- #

INCLUDES        := -I$(INCDIR)

# -=-=-=-=-    FLAGS -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- #

CC              := c++
CFLAGS          := -Wall -Wextra -Werror -std=c++20 -g3 -O0 $(INCLUDES)
DEPFLAGS        := -MMD -MP

# Raylib flags (adjust if using system raylib)
RAYLIB_INCLUDES := $(shell pkg-config --cflags raylib 2>/dev/null || echo "-I/usr/local/include")
RAYLIB_LIBS     := $(shell pkg-config --libs raylib 2>/dev/null || echo "-lraylib -lm -lpthread -ldl -lrt -lX11")

# SDL2 flags (for particle system - will remove after porting)
SDL_INCLUDES    := $(shell pkg-config --cflags sdl2 2>/dev/null || echo "")
SDL_LIBS        := $(shell pkg-config --libs sdl2 2>/dev/null || echo "-lSDL2")

ALL_INCLUDES    := $(INCLUDES) $(RAYLIB_INCLUDES) $(SDL_INCLUDES)
ALL_LIBS        := $(RAYLIB_LIBS) $(SDL_LIBS)

# -=-=-=-=-    RULES -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- #

.PHONY: all clean fclean re

all: $(NAME)

$(NAME): $(OBJS)
	@echo "$(CYAN)Linking $(NAME)...$(DEF_COLOR)"
	@$(CC) $(CFLAGS) $(OBJS) -o $(NAME) $(ALL_LIBS)
	@echo "$(GREEN)✓ Built $(NAME)$(DEF_COLOR)"

# Object file compilation
$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	@mkdir -p $(dir $@)
	@mkdir -p $(dir $(DEPDIR)/$*.d)
	@echo "$(YELLOW)Compiling $<...$(DEF_COLOR)"
	@$(CC) $(CFLAGS) $(ALL_INCLUDES) $(DEPFLAGS) -c $< -o $@ -MF $(DEPDIR)/$*.d

# Include dependencies
-include $(DEPS)

# -=-=-=-=-    CLEANING -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- #

clean:
	@echo "$(RED)Cleaning object files...$(DEF_COLOR)"
	@rm -rf $(OBJDIR) $(DEPDIR)

fclean: clean
	@echo "$(RED)Cleaning $(NAME)...$(DEF_COLOR)"
	@rm -f $(NAME)

re: fclean all

# -=-=-=-=-    TESTING -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- #

test: all
	@echo "$(BLUE)Running tests...$(DEF_COLOR)"
	@if [ -d "tests" ]; then \
		cd tests && cmake -B build && cmake --build build && cd build && ctest --output-on-failure; \
	else \
		echo "$(YELLOW)No tests directory found$(DEF_COLOR)"; \
	fi

# -=-=-=-=-    DEBUG INFO -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- #

info:
	@echo "$(CYAN)=== Build Configuration ===$(DEF_COLOR)"
	@echo "Name:     $(NAME)"
	@echo "Sources:  $(words $(SRCS)) files"
	@echo "Compiler: $(CC)"
	@echo "Flags:    $(CFLAGS)"
	@echo "Raylib:   $(RAYLIB_LIBS)"
	@echo "SDL2:     $(SDL_LIBS)"
