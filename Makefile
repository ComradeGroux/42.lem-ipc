GREENGREEN = \033[38;5;46m
RED = \033[0;31m
GREY = \033[38;5;240m
RESET = \033[0m

NAME	= lemipc


CC			= gcc
CFLAGS		= -Wall -Wextra -Werror -O3
DEBUG_FLAG  = -DDEBUG -g -fsanitize=address

RM			= rm -rf

INC_DIR		= include
SRC_DIR		= src
LIB_DIR		= lib

BUILD_DIR	= build
OBJ_DIR		= ${BUILD_DIR}/obj

SRCS_LIST 	=	log.c \
				main.c \
				shared_resources.c

SRCS	:= ${addprefix ${SRC_DIR}/, ${SRCS_LIST}}
VPATH	:= $(dir $(SRCS))

OBJS	:= $(addprefix $(OBJ_DIR)/, $(notdir $(SRCS:.c=.o)))

DIR_LIBFT = ${LIB_DIR}/libft/
LIBFT_INC = -I ${DIR_LIBFT}
LIBFT =	${DIR_LIBFT}libft.a
FT_LNK = -L ${DIR_LIBFT} -lft

LIBS = ${FT_LNK}

${NAME}: ${BUILD_DIR} ${LIBFT} ${OBJS}
	@echo "$(RESET)[$(GREENGREEN)${NAME}$(RESET)]: ${NAME} Objects were created${GREY}"
	${CC} ${CFLAGS} ${OBJS} ${LIBS} -o ${NAME}
	@echo "$(RESET)[$(GREENGREEN)${NAME}$(RESET)]: ${NAME} created !"

${LIBFT}:
	@echo "[$(GREENGREEN)${NAME}$(RESET)]: Creating Libft...${GREY}"
	${MAKE} -sC ${DIR_LIBFT} BUILD_DIR=../../${BUILD_DIR}/libft
	@echo "$(RESET)[$(GREENGREEN)${NAME}$(RESET)]: Libft Objects were created"

${BUILD_DIR}:
	mkdir -p ${BUILD_DIR}
	mkdir -p ${BUILD_DIR}/libft
	mkdir -p ${OBJ_DIR}

all: ${NAME}

debug: CFLAGS += ${DEBUG_FLAG}
debug: all


${OBJ_DIR}%.o:${SRC_DIR}%.c
	@printf "\033[38;5;240m"
	${CC} ${CFLAGS} ${LIBFT_INC} -I${INC_DIR} -o $@ -c $<

clean:
	@echo "[$(RED)${NAME}$(RESET)]: Cleaning ${NAME} Objects...${GREY}"
	${RM} ${OBJ_DIR}
	@echo "[$(RED)${NAME}$(RESET)]: ${NAME} Objects were cleaned${GREY}"

libclean:
	@echo "${RESET}[$(RED)${NAME}$(RESET)]: Cleaning Libft...${GREY}"
	${MAKE} -sC ${DIR_LIBFT} fclean
	@echo "${RESET}[$(RED)${NAME}$(RESET)]: Libft Objects were cleaned"

fclean: clean libclean
	@echo "${RESET}[$(RED)${NAME}$(RESET)]: Cleaning ${NAME}...${GREY}"
	${RM} ${NAME}
	${RM} ${BUILD_DIR}
	@echo "${RESET}[$(RED)${NAME}$(RESET)]: ${NAME} was cleaned"

re: fclean all

.PHONY: all clean fclean re libclean
