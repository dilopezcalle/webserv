NAME			=	webserv
LIBFT			=	libft.a
SRC_DIR			=	src
OBJ_DIR 		=	obj
INC_DIR			=	include

RM				=	rm -rf
CC				=	c++
CFLAGS			=	-I $(INC_DIR) -Wall -Werror -Wextra

SRCS			=	$(SRC_DIR)/web_server.cpp	\

OBJS			=	$(SRCS:.cpp=.o)
PREFIXED		=	$(addprefix $(OBJ_DIR)/, $(OBJS))

RED				=	"\\x1b[31m"
GREEN			=	"\\x1b[32m"
YELLOW			=	"\\x1b[33m"
RESET			=	"\\x1b[37m"

# =================================== RULES ===================================

$(NAME):	$(PREFIXED)
	@printf \
	"$(YELLOW)-------------- Project's files compiled --------------\n$(RESET)\n%-33.33s\r"
	@make -C ./$(SRC_DIR)/libft
	@cp ./$(SRC_DIR)/libft/$(LIBFT) .
	@$(CC) $(CFLAGS) $(PREFIXED) $(LIBFT) -o $(NAME)
	@echo "\n${GREEN}-------------- Mandatory part compiled! --------------${RESET}\n"

$(OBJ_DIR)/%.o:	%.cpp
	@mkdir -p $(OBJ_DIR)/$(SRC_DIR)
	@printf "$(YELLOW)Compiling project's file -> $(RESET)%-33.33s\r" $<
	@$(CC) $(CFLAGS) -c $< -o $@

# ================================= FUNCTIONS =================================

all:	$(NAME)

clean:
	@make -C ./src/libft clean
	@$(RM) $(OBJ) $(OBJ_DIR)
	@echo "\n${GREEN}---------------------- Cleaned! ----------------------${RESET}\n"

fclean:	clean
	@make -C ./src/libft fclean
	@$(RM) $(NAME) $(LIBFT) $(NAME).dSYM

re:		fclean all

.PHONY: all clean fclean re bonus print_init