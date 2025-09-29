MAKEFLAGS   += --no-print-directory

NAME        = FT_IRC
CC          = c++
CFLAGS      = -Wall -Wextra -Werror -g3 -std=c++98 -MMD

# Folders
INCLUDES    = includes/
SRCS_DIR    = srcs/
OBJ_DIR     = obj/

RM          = rm -rf

# Colors
DEF_COLOR   = \033[0;39m
MAGENTA     = \033[0;95m
GREEN       = \033[0;92m
CYAN        = \033[0;96m
RESET_LINE  = \033[A\033[K

# Files
SRC_FILE    = user.cpp server.cpp main.cpp utils.cpp message.cpp channel.cpp tools.cpp
SRCS        = $(addprefix $(SRCS_DIR), $(SRC_FILE))
OBJ         = $(patsubst $(SRCS_DIR)%.cpp, $(OBJ_DIR)%.o, $(SRCS))
DEP         = $(OBJ:.o=.d)

# Rules
all: $(NAME)

$(NAME): $(OBJ)
	@$(CC) -o $(NAME) $(CFLAGS) -I$(INCLUDES) $(OBJ)
	@echo "$(RESET_LINE)$(MAGENTA)[ $(NAME) ]$(DEF_COLOR) COMPILED $(GREEN)✔$(DEF_COLOR)"

$(OBJ_DIR)%.o: $(SRCS_DIR)%.cpp | $(OBJF)
	@mkdir -p $(dir $@)
	@$(CC) $(CFLAGS) -I$(INCLUDES) -c $< -o $@
	@echo "$(RESET_LINE)$(MAGENTA)[ $(NAME) ]$(DEF_COLOR) compiling: $< $(GREEN)✔$(DEF_COLOR)"

$(OBJF):
	@mkdir -p $(OBJ_DIR)

clean:
	@$(RM) $(OBJ_DIR)
	@echo "$(RESET_LINE)$(CYAN)Object files cleared$(DEF_COLOR)"

fclean: clean
	@$(RM) $(NAME)
	@echo "$(RESET_LINE)$(CYAN)Everything cleared!$(DEF_COLOR)"

re: fclean all
	@echo "$(GREEN)Everything has been rebuilt, fresh and clean$(DEF_COLOR)"

-include $(DEP)

.PHONY: all clean fclean reMAKEFLAGS   += --no-print-directory