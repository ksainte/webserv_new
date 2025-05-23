# Executable name
NAME     := webserv

# Directories
BIN_DIR  := bin
DEP_DIR  := dep
INCLUDE  := includes
SRCS_DIR := srcs

# Compiler and flags
CC       := c++
CFLAGS   := -Wall \
            -Wextra \
            -Werror \
            -g \
            -std=c++98 \
            -I$(INCLUDE)

# Source files, object files, and dependency files
SRCS := $(shell find $(SRCS_DIR) -name '*.cpp')
OBJS := $(SRCS:$(SRCS_DIR)/%.cpp=$(BIN_DIR)/%.o)
DEPS := $(SRCS:$(SRCS_DIR)/%.cpp=$(DEP_DIR)/%.d)

# --- Parallel Compilation Setup ---
# Attempt to determine the number of CPU cores for parallel compilation.
# Defaults to 1 if detection fails.
ifndef NPROC
  NPROC := $(shell nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 1)
endif

# GNU Make specific: Set the default number of parallel jobs.
# This tells Make to try and run NPROC jobs in parallel.
# If NPROC is 1, it will run sequentially.
# Users can override this by passing -jN to the make command (e.g., make -j1 for serial).
ifneq ($(NPROC),1)
  .JOBS := $(NPROC)
  $(info Building with up to $(NPROC) parallel jobs (GNU Make). Override with 'make -jN'.)
else
  $(info Building sequentially (1 core detected or detection failed, or NPROC explicitly set to 1).)
endif
# --- End Parallel Compilation Setup ---

# Default target
all: $(NAME)

# Link the executable
$(NAME): $(OBJS)
	@$(CC) $(CFLAGS) -o $(NAME) $(OBJS)
	@printf "Linking: $(NAME)\n"

# Include the generated dependency files
# The '-' before include suppresses errors if the files don't exist (e.g., on first build)
-include $(DEPS)

# Compile source files and generate dependency files
# $@: target file name (e.g., bin/main.o)
# $<: first prerequisite name (e.g., srcs/main.cpp)
# -MMD: Generate dependencies, excluding system headers
# -MP: Create phony targets for prerequisites, avoids errors if headers are removed
# -MF <file>: Write dependencies to <file>
$(BIN_DIR)/%.o: $(SRCS_DIR)/%.cpp Makefile
	@mkdir -p $(dir $@)
	@mkdir -p $(DEP_DIR)
	@$(CC) $(CFLAGS) -MMD -MP -c $< -o $@ -MF $(DEP_DIR)/$(notdir $(basename $<)).d
	@printf "Compiling: $(notdir $<) -> $@ (deps: $(DEP_DIR)/$(notdir $(basename $<)).d)\n"

# Clean object files and dependency files
clean:
	@rm -rf $(BIN_DIR) $(DEP_DIR)
# @printf "Cleaning: $(BIN_DIR) and $(DEP_DIR)\n"

# Clean everything including the executable
fclean: clean
	@rm -f $(NAME)
# @printf "Cleaning: $(NAME)\n"

# Rebuild everything
re: fclean all

# Phony targets (targets that are not actual files)
.PHONY: all clean fclean re