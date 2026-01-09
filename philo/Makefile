# philo/Makefile

CC			= cc
CFLAGS		= -Wall -Wextra -Werror
NAME		= philo

SRCDIR		= src
INCDIR		= includes
OBJDIR		= ../build/mandatory

SOURCES		= main.c init.c actions.c philosopher.c time.c utils.c
OBJECTS		= $(addprefix $(OBJDIR)/,$(SOURCES:.c=.o))

INCLUDES	= -I$(INCDIR)

.PHONY: all clean fclean re

all: $(NAME)

$(OBJDIR)/%.o: $(SRCDIR)/%.c | $(OBJDIR)
	@$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(OBJDIR):
	@mkdir -p $(OBJDIR)

$(NAME): $(OBJECTS)
	@$(CC) $(CFLAGS) $(OBJECTS) -o $(NAME) -lpthread

clean:
	@rm -rf $(OBJDIR)

fclean: clean
	@rm -f $(NAME)

re: fclean all
