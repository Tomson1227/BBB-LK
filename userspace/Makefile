NAME		= DistanceMeasure

SRC_DIR		= src/
OBJ_DIR		= obj/
INC_DIR		= inc/

SRC			= $(wildcard $(SRC_DIR)*.c)
OBJ			= $(SRC:src/%.c=$(OBJ_DIR)%.o)
INC_H		= $(wildcard $(INC_DIR)*.h)

CC			= gcc
GFLAGS		= -std=c11 -Wall -Wextra -Werror -Wpedantic \
	-Wno-unused-command-line-argument -Wno-unused-variable \
	-Wno-unused-function -Wno-unused-parameter

IFLAGS		= $(addprefix -I, $(LIB_INC) $(INC_DIR))

COMPILE		= $(CC) $(GFLAGS) $(IFLAGS)
MAKE_M		= make -sf Makefile -C
RM			= /bin/rm -rf

all: $(NAME)

$(NAME): $(LIB_LIST) $(OBJ_DIR) $(OBJ) 
	@$(COMPILE) $(OBJ) -o $(NAME)
	@printf "\r\33[2K$@ \033[35;1mcreated\033[0m\n"

$(OBJ_DIR):
	@mkdir -p $@ $(foreach dir, $(DIRS), $@/$(dir))

$(OBJ_DIR)%.o: $(SRC_DIR)%.c $(INC_H) 
	@$(COMPILE) -o $@ -c $<
	@printf "\r\33[2K$(NAME) \033[37;1mcompile \033[0m$(<:$(SRC_DIR)/%.c=%)"

clean:
	@$(RM) $(OBJ_DIR)
	@printf "$(NAME) fs \033[36;1mcleaned\033[0m\n"

uninstall:
	@$(RM) $(OBJ_DIR) $(NAME)
	@printf "$(NAME) \033[36;1muninstalled\033[0m\n"

reinstall: uninstall all

.PHONY: all clean uninstall reinstall $(LIB_LIST)