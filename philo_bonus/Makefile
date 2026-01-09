# philo_bonus/Makefile

CC			= cc
CFLAGS		= -Wall -Wextra -Werror
NAME		= philo_bonus

SRCDIR		= .
OBJDIR		= ../build/bonus

# Add your .c files here when ready (e.g., main.c, routine.c, etc.)
SOURCES		= main.c init.c routine.c utils.c cleanup.c
OBJECTS		= $(addprefix $(OBJDIR)/,$(SOURCES:.c=.o))

.PHONY: all clean fclean re

all: $(NAME)

$(OBJDIR)/%.o: $(SRCDIR)/%.c | $(OBJDIR)
	@$(CC) $(CFLAGS) -c $< -o $@

$(OBJDIR):
	@mkdir -p $(OBJDIR)

$(NAME): $(OBJECTS)
	@$(CC) $(CFLAGS) $(OBJECTS) -o $(NAME)

clean:
	@rm -rf $(OBJDIR)

fclean: clean
	@rm -f $(NAME)

re: fclean all
