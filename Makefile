# Colors for output
GREEN = \033[1;32m
YELLOW = \033[1;33m
RED = \033[1;31m
RESET = \033[0m

# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -Werror -pthread

# Directories
MANDATORY_DIR = philo
BONUS_DIR = philo_bonus
MANDATORY_SRC_DIR = $(MANDATORY_DIR)/src
MANDATORY_INC_DIR = $(MANDATORY_DIR)/includes
BONUS_SRC_DIR = $(BONUS_DIR)/src
BONUS_INC_DIR = $(BONUS_DIR)/includes
BUILD_DIR = build

# Sources and objects
MANDATORY_SRCS = $(wildcard $(MANDATORY_SRC_DIR)/*.c)
BONUS_SRCS = $(wildcard $(BONUS_SRC_DIR)/*.c)
MANDATORY_OBJS = $(MANDATORY_SRCS:$(MANDATORY_SRC_DIR)/%.c=$(BUILD_DIR)/mandatory/%.o)
BONUS_OBJS = $(BONUS_SRCS:$(BONUS_SRC_DIR)/%.c=$(BUILD_DIR)/bonus/%.o)

# Targets
MANDATORY_EXEC = philo
BONUS_EXEC = philo_bonus

# Default target
all: $(MANDATORY_EXEC)

# Build mandatory
$(MANDATORY_EXEC): $(MANDATORY_OBJS)
	@echo -e "$(GREEN)Linking mandatory executable $(MANDATORY_EXEC)...$(RESET)"
	$(CC) $(CFLAGS) $(MANDATORY_OBJS) -o $(MANDATORY_EXEC)

# Build bonus
bonus: $(BONUS_EXEC)

$(BONUS_EXEC): $(BONUS_OBJS)
	@echo -e "$(GREEN)Linking bonus executable $(BONUS_EXEC)...$(RESET)"
	$(CC) $(CFLAGS) $(BONUS_OBJS) -o $(BONUS_EXEC)

# Compile object files
$(BUILD_DIR)/mandatory/%.o: $(MANDATORY_SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -I$(MANDATORY_INC_DIR) -c $< -o $@

$(BUILD_DIR)/bonus/%.o: $(BONUS_SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -I$(BONUS_INC_DIR) -c $< -o $@

# Cleaning rules
clean:
	@echo -e "$(YELLOW)Cleaning object files...$(RESET)"
	rm -rf $(BUILD_DIR)

fclean: clean
	@echo -e "$(RED)Cleaning all executables...$(RESET)"
	rm -f $(MANDATORY_EXEC) $(BONUS_EXEC)

re: fclean all

# Phony targets
.PHONY: all bonus clean fclean re
