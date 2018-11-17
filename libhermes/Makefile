# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: rlutt <rlutt@student.42.us.org>            +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2016/11/07 13:28:20 by rlutt             #+#    #+#              #
#    Updated: 2018/09/16 15:21:56 by rlutt            ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = libhermes.a

CC = gcc

OBJ = obj/*.o

CCFLAGS = -Wall -Wextra -c -g -fsanitize=address

RM = -rm -fr

ARCHIVE = ar rc

INDEX = ranlib

HEADDR = incl/libhermes.h

FILES = list.c bst.c node.c memory.c str_tbl.c hermes_error.c swap.c pow_round.c
FILES += hashtbl.c

SRC_DIR = src

SRC = $(addprefix $(SRC_DIR)/, $(FILES))

$(NAME):
		$(shell mkdir obj)
		$(CC) $(CCFLAGS) $(SRC)
		mv *.o obj
		$(ARCHIVE) $(NAME) $(OBJ)
		$(INDEX) $(NAME)

all: $(NAME)

clean:
		$(RM) $(OBJ)

fclean: clean
		$(RM) $(NAME)

re: fclean $(NAME)
