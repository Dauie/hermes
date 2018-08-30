
NAME = nmap

CC = gcc

SRCDIR = src

CFLAGS  = -Wall -Werror -Wextra

INCL = -I incl

LIBFT = ./libft/libftprintf.a

SRC = traceroute.c send_recv_echo.c che

SRCFILES = $(addprefix $(SRCDIR)/, $(SRC))

RM = rm -fr

$(NAME):
		$(MAKE) -C ./libft/ re
		$(CC) $(CFLAGS) $(INCL) $(SRCFILES) $(LIBFT) -o $(NAME)

clean:
		$(RM) $(OBJDIR)

fclean: clean
		$(RM) $(NAME)

re: fclean $(NAME)
