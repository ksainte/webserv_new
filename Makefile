
NAME     := webserv
BIN      := bin
INCLUDE  := includes
SRCS_DIR := srcs

CFLAGS   := -Wall \
            -Wextra \
            -g \
            -std=c++98 \
            -I$(INCLUDE)

CC       := c++
SRCS     := $(shell find $(SRCS_DIR) -name '*.cpp')
OBJS     := $(patsubst $(SRCS_DIR)/%.cpp, $(BIN)/%.o, $(SRCS))
DEPS     := $(OBJS:.o=.d)

# Default target
all: $(NAME)

# Link the executable
$(NAME): $(OBJS)
	@$(CC) $(CFLAGS) -o $(NAME) $(OBJS)
	@printf "Linking: $(NAME)\n"

-include $(DEPS)

# Compile source files and generate dependency files
$(BIN)/%.o: $(SRCS_DIR)/%.cpp Makefile
	@mkdir -p $(dir $@)
	@$(CC) $(CFLAGS) -MMD -MP -c $< -o $@
	@printf "Compiling: $(notdir $<)\n"

clean:
	rm -rf $(BIN)
	printf "Cleaning: $(BIN)\n"

# Clean everything including the binary
fclean: clean
	rm -f $(NAME)
	@printf "Cleaning: $(NAME)\n"

# Rebuild everything
re: fclean all

.PHONY: all clean fclean re

