NAME = codexion
CC = cc

SRC_DIR = src
OBJ_DIR = .obj
INC_DIR = include

CFLAGS = -Wall -Wextra -Werror -pthread -I $(INC_DIR)
INCLUDE = $(INC_DIR)/codexion.h

SRC_FILES = main.c argparse.c helpfunc.c init.c routine.c heap.c simulation.c dongle.c monitor.c acquire_two_dongles.c helpfunc2.c

SRC = $(addprefix $(SRC_DIR)/, $(SRC_FILES))
OBJ = $(addprefix $(OBJ_DIR)/, $(SRC_FILES:.c=.o))

all: $(NAME)

$(NAME): $(OBJ)
	@$(CC) $(CFLAGS) $(OBJ) -o $(NAME)
	@echo "$(NAME) created successfully"

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c $(INCLUDE)
	@mkdir -p $(OBJ_DIR)
	@$(CC) $(CFLAGS) -c $< -o $@

clean:
	@rm -rf $(OBJ_DIR)
	@echo "Removing object files"

fclean: clean
	@rm -f $(NAME)
	@echo "Removing executable"

re: fclean all

norm:
	norminette -R CheckForbiddenSourceHeader .

.PHONY: all clean fclean re norm
