# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: jlima-so <jlima-so@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/06/12 13:07:24 by jlima-so          #+#    #+#              #
#    Updated: 2025/07/06 16:27:12 by jlima-so         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

PROJ=philo

PROJ_BONUS=	${PROJ}_bonus

SUBJ_DIR=${PROJ}_dir/

BONUS_DIR=${PROJ}_bonus_dir/

#SRC_LIB=	$(wildcard ${SRC_DIR1}*.c) \
#			$(wildcard ${SRC_DIR2}*.c) \
#			$(wildcard ${SRC_DIR3}*.c)

#OBJ_LIB=	${SRC_LIB:.c=.o}

SRC_FILES=  $(wildcard ${SUBJ_DIR}*.c) \
			$(wildcard listas/*.c) \


OBJ_FILES=	${SRC_FILES:.c=.o}

SRC_BONUS=	$(wildcard ${BONUS_DIR}*.c)

OBJ_BONUS=	${SRC_BONUS:.c=.o}

NAME=${PROJ}.a

LIBFT=libft.a

BONUS_ARQ=bonus.a

HDR_FILE=libft.h

CC=cc

AR=ar rcs

CFLAGS= -pthread -g -Wall -Wextra -Werror 

all: ${PROJ}

${NAME}: ${OBJ_FILES}
	${AR} ${NAME} $?

#${LIBFT}: ${OBJ_LIB}
#	cd my_libft && make

#${BONUS_ARQ}: ${OBJ_BONUS}
#	${AR} ${BONUS_ARQ} $?

${PROJ}: ${NAME} ${PROJ}.c
	${CC} ${CFLAGS} ${PROJ}.c ${NAME} -o ${PROJ}

${PROJ_BONUS}: ${NAME} ${PROJ_BONUS}.c
	${CC} ${CFLAGS} ${PROJ_BONUS}.c ${NAME} -o ${PROJ_BONUS}

%o: %c
	${CC} ${CFLAGS} $@ $<

bonus: ${PROJ_BONUS}
	@touch bonus

clean:
	rm -f ${OBJ_FILES} ${OBJ_BONUS} ${OBJ_LIB} ${BONUS_ARQ} ${NAME} ${LIBFT} bonus

fclean: clean
	rm -f ${NAME} ${LIBFT} ${PROJ} ${PROJ_BONUS}

re: fclean all

.PHONY: re fclean clean all

# so:
#	$(CC) -nostartfiles -fPIC $(CFLAGS) $(SRC_FILES)
#	gcc -nostartfiles -shared -o libft.so $(OBJ_FILES)
# 