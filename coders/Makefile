NAME = codexion
CC = cc
CFLAGS = -Wall -Wextra -Werror -pthread
INCLUDE = include/codexion.h

SRC = src/main.c \
	src/argparse.c \
	src/helpfunc.c \
	src/init.c \
	src/routine.c \
	src/heap.c \
	src/simulation.c \
	src/dongle.c \
	src/monitor.c \
	src/acquire_two_dongles.c

OBJ = $(SRC:.c=.o)

all: $(NAME)

$(NAME): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $(NAME)

%.o: %.c $(INCLUDE)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ)

fclean: clean
	rm -f $(NAME)

re: fclean all

norm:
	norminette -R CheckForbiddenSourceHeader .

.PHONY: all clean fclean re norm
