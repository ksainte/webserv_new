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
SRCS     := $(shell find $(SRCS_DIR) -name '*.cpp')
OBJS     := $(patsubst $(SRCS_DIR)/%.cpp, $(BIN_DIR)/%.o, $(SRCS))
DEPS     := $(patsubst $(SRCS_DIR)/%.cpp, $(DEP_DIR)/%.d, $(SRCS))

# Default target
all: $(NAME)

# Link the executable
$(NAME): $(OBJS)
	@$(CC) $(CFLAGS) -o $(NAME) $(OBJS)
	@printf "Linking: \033[0;32m$(NAME)\033[0m\n"

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
	@mkdir -p $(dir $@) # Ensure the object file directory exists (e.g., bin/subdir/)
	@mkdir -p $(dir $(patsubst $(SRCS_DIR)/%.cpp, $(DEP_DIR)/%.d, $<)) # Ensure the dependency file directory exists (e.g., dep/subdir/)
	@$(CC) $(CFLAGS) -MMD -MP -c $< -o $@ -MF $(patsubst $(SRCS_DIR)/%.cpp, $(DEP_DIR)/%.d, $<)
	@printf "Compiling: \033[0;33m$(notdir $<)\033[0m -> \033[0;36m$@\033[0m (deps: \033[0;35m$(patsubst $(SRCS_DIR)/%.cpp, $(DEP_DIR)/%.d, $<)\033[0m)\n"

# Clean object files and dependency files
clean:
	@rm -rf $(BIN_DIR) $(DEP_DIR)
	@printf "Cleaning: \033[0;31m$(BIN_DIR)\033[0m and \033[0;31m$(DEP_DIR)\033[0m\n"

# Clean everything including the executable
fclean: clean
	@rm -f $(NAME)
	@printf "Cleaning: \033[0;31m$(NAME)\033[0m\n"

# Rebuild everything
re: fclean all

# Phony targets (targets that are not actual files)
.PHONY: all clean fclean re
