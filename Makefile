NAME = philo

CC = gcc
CFLAGS = -Wall -Wextra -Werror -pthread

UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Linux)
    CFLAGS += -D_DEFAULT_SOURCE
endif

SRCS = src/main.c src/init.c src/philosopher.c src/actions.c src/utils.c src/time.c

OBJS = $(SRCS:.c=.o)

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(NAME)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re