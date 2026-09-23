NAME = codexion.o
CC = cc
CFLAGS = -Wall -Wextra -Werror
INCLUDE = coders/include/codexion.h

SRC = coders/src/argparse.c \

OBJ = $(SRC:.c=.o)

all: $(NAME)
	$(CC) coders/src/main.c $(NAME) $(CFLAGS) -o codexion

$(NAME): $(OBJ)
	ar rcs $(NAME) $(OBJ)

%.o: %.c $(INCLUDE)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJ)

fclean: clean
	rm -rf $(NAME)
	rm -rf codexion

re: fclean all

norm:
	norminette -R CheckForbiddenSourceHeader .

.PHONY: all clean fclean re