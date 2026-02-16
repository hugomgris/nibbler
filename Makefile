# -=-=-=-=-    COLOURS -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-════=-=-=-=-=-=-=-=- #

DEF_COLOR   = \033[0;39m
YELLOW      = \033[0;93m
CYAN        = \033[0;96m
GREEN        = \033[0;92m
BLUE        = \033[0;94m
RED         = \033[0;91m

# -=-=-=-=-    NAMES -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- #

NAME                := rosario

# -=-=-=-=-    DIRECTORIES -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- #

SRCDIR          := srcs
OBJDIR          := .obj
DEPDIR          := .dep
INCDIR          := incs

# -=-=-=-=-    SOURCE FILES -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- #

MAIN_SRC        := main.cpp
CORE_SRC        := core/GameController.cpp \
					core/InputManager.cpp \
					core/Snake.cpp \
					core/Food.cpp \
					core/Utils.cpp

AI_SRC          := AI/AIConfig.cpp \
					AI/FloodFill.cpp \
					AI/Pathfinder.cpp \
					AI/SnakeAI.cpp \
					AI/GridHelper.cpp

GRAPHICS_SRC    := graphics/Renderer.cpp \
					graphics/TextSystem.cpp \
					graphics/ParticleSystem.cpp \
					graphics/AnimationSystem.cpp \
					graphics/MenuSystem.cpp \
					graphics/RaylibColors.cpp \
					graphics/PostProcessingSystem.cpp

ALL_SRC         := $(MAIN_SRC) $(CORE_SRC) $(AI_SRC) $(GRAPHICS_SRC)

SRCS            := $(addprefix $(SRCDIR)/, $(ALL_SRC))
OBJS            := $(addprefix $(OBJDIR)/, $(ALL_SRC:.cpp=.o))
DEPS            := $(addprefix $(DEPDIR)/, $(ALL_SRC:.cpp=.d))

# -=-=-=-=-    INCLUDES -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- #

INCLUDES        := -I$(INCDIR)

# -=-=-=-=-    FLAGS -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- #

CC              := c++
CFLAGS := -std=c++20 -g3 -O0 -Wall -Wextra \
			-Wno-unused-parameter \
			-Wno-unused-variable \
			-Wno-sign-compare \
			$(INCLUDES)

# PRODUCTION FLAGS#
#CFLAGS := -std=c++20 -g3 -O0 -Wall -Wextra -Werror \
			-Wno-unused-parameter \
			$(INCLUDES)

DEPFLAGS        := -MMD -MP

# -=-=-=-=-    EXTERNAL LIBRARIES -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- #

LIB_DIR         := libs
RAYLIB_DIR      := $(LIB_DIR)/raylib
RAYLIB_SRC_DIR  := $(RAYLIB_DIR)/src
RAYLIB_REPO     := https://github.com/raysan5/raylib.git
RAYLIB_VERSION  := 5.0

# Raylib flags (local build)
RAYLIB_INCLUDES := -I$(RAYLIB_SRC_DIR) -Wno-missing-field-initializers
RAYLIB_LIBS     := -L$(RAYLIB_SRC_DIR) -lraylib -lm -lpthread -ldl -lrt -lX11

ALL_INCLUDES    := $(INCLUDES) $(RAYLIB_INCLUDES)
ALL_LIBS        := $(RAYLIB_LIBS)

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

TEST_CFLAGS		:= $(CFLAGS) $(GTEST_INCLUDES) $(RAYLIB_INCLUDES)
TEST_LDFLAGS	:= -lpthread $(ALL_LIBS)

# -=-=-=-=-    RULES -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- #

all: $(RAYLIB_SRC_DIR)/libraylib.a $(NAME)

game: re
	./rosario 16 16

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

check_raylib:
	@if [ ! -f "$(RAYLIB_SRC_DIR)/libraylib.a" ]; then \
		echo "$(YELLOW)Raylib not found. Building...$(DEF_COLOR)"; \
		$(MAKE) $(RAYLIB_SRC_DIR)/libraylib.a; \
	else \
		echo "$(GREEN)Raylib already built$(DEF_COLOR)"; \
	fi

$(RAYLIB_SRC_DIR)/libraylib.a:
	@echo "$(YELLOW)Raylib not found. Cloning and building...$(DEF_COLOR)"
	@mkdir -p $(LIB_DIR)
	@if [ ! -d "$(RAYLIB_DIR)" ]; then \
		echo "$(CYAN)Cloning Raylib $(RAYLIB_VERSION)...$(DEF_COLOR)"; \
		git clone --depth 1 --branch $(RAYLIB_VERSION) $(RAYLIB_REPO) $(RAYLIB_DIR); \
	fi
	@echo "$(CYAN)Building Raylib...$(DEF_COLOR)"
	@if [ -d "$(RAYLIB_SRC_DIR)" ]; then \
		cd $(RAYLIB_SRC_DIR) && $(MAKE) PLATFORM=PLATFORM_DESKTOP; \
	else \
		echo "$(RED)ERROR: Raylib source directory not found!$(DEF_COLOR)"; \
		exit 1; \
	fi
	@echo "$(GREEN)✓ Raylib built successfully$(DEF_COLOR)"

$(NAME): $(OBJS) Makefile
	@echo "$(CYAN)Linking $(NAME)...$(DEF_COLOR)"
	@$(CC) $(CFLAGS) $(OBJS) -o $(NAME) $(ALL_LIBS)
	@echo "$(GREEN)Snakeboarding is not a crime!$(DEF_COLOR)"

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	@mkdir -p $(dir $@)
	@mkdir -p $(dir $(DEPDIR)/$*.d)
	@echo "$(YELLOW)Compiling $<...$(DEF_COLOR)"
	@$(CC) $(CFLAGS) $(ALL_INCLUDES) $(DEPFLAGS) -c $< -o $@ -MF $(DEPDIR)/$*.d

-include $(DEPS)

# -=-=-=-=-    TEST TARGETS -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- #

test: check_gtest check_raylib $(TEST_BINARY)
	@echo "$(CYAN)Running tests...$(DEF_COLOR)"
	./$(TEST_BINARY)

$(TEST_BINARY): $(TESTABLE_OBJS) $(TEST_OBJS) $(GTEST_LIB) $(GTEST_MAIN_LIB)
	@echo "$(YELLOW)Linking test binary...$(DEF_COLOR)"
	$(CC) -o $@ $(TESTABLE_OBJS) $(TEST_OBJS) $(GTEST_LIB) $(GTEST_MAIN_LIB) $(TEST_LDFLAGS)
	@chmod +x $@
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

# -=-=-=-=-    CLEANING -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- #

clean:
	@echo "$(RED)Cleaning object files...$(DEF_COLOR)"
	@rm -rf $(OBJDIR) $(DEPDIR) $(TEST_DEPDIR)

fclean: clean
	@echo "$(RED)Cleaning $(NAME)...$(DEF_COLOR)"
	@rm -rf $(LIB_DIR)
	@rm -f $(NAME) $(TEST_BINARY)
	@rm -rf $(TEST_OBJDIR)

cleanlibs:
	@echo "$(RED)Cleaning Raylib...$(DEF_COLOR)"
	@rm -rf $(LIB_DIR)

re: fclean all

# -=-=-=-=-    DEBUG INFO -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- #

info:
	@echo "$(CYAN)=== Build Configuration ===$(DEF_COLOR)"
	@echo "Name:     $(NAME)"
	@echo "Sources:  $(words $(SRCS)) files"
	@echo "Compiler: $(CC)"
	@echo "Flags:    $(CFLAGS)"
	@echo "Raylib:   $(RAYLIB_LIBS)"
	@echo "SDL2:     $(SDL_LIBS)"

.PHONY: all clean fclean re test check_gtest check_raylib