# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: rlutt <rlutt@student.42.us.org>            +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2016/11/07 13:28:20 by rlutt             #+#    #+#              #
#    Updated: 2018/08/27 14:06:17 by rlutt            ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = libhermese.a

CC = gcc

OBJ = obj/*.o

CCFLAGS = -I.incl -Wall -Wextra -c

RM = -rm -fr

ARCHIVE = ar rc

INDEX = ranlib

HEADDR = incl/libhermese.h

FILES = list.c memory.c str_tbl.c

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
