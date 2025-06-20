NAME     := webserv

BIN_DIR  := bin
DEP_DIR  := dep
INCLUDE  := includes
SRCS_DIR := srcs

CC       := c++
CFLAGS   := -Wall \
            -Wextra \
			-Werror \
            -fsanitize=address \
			-g \
            -std=c++98 \
            -I$(INCLUDE)

SRCS := $(shell find $(SRCS_DIR) -name '*.cpp')
OBJS := $(SRCS:$(SRCS_DIR)/%.cpp=$(BIN_DIR)/%.o)
DEPS := $(SRCS:$(SRCS_DIR)/%.cpp=$(DEP_DIR)/%.d)

all: $(NAME)

$(NAME): $(OBJS)
	@$(CC) $(CFLAGS) -o $(NAME) $(OBJS)
	@printf "Linking: $(NAME)\n"

-include $(DEPS)

$(BIN_DIR)/%.o: $(SRCS_DIR)/%.cpp Makefile
	@mkdir -p $(dir $@)
	@mkdir -p $(DEP_DIR)
	@$(CC) $(CFLAGS) -MMD -MP -c $< -o $@ -MF $(DEP_DIR)/$(notdir $(basename $<)).d
	@printf "Compiling: $(notdir $<) -> $@ (deps: $(DEP_DIR)/$(notdir $(basename $<)).d)\n"

clean:
	@rm -rf $(BIN_DIR) $(DEP_DIR)

fclean: clean
	@rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re