
NAME = hermes

CC = gcc

SRC_DIR = src

CFLAGS  = -Wall -Werror -Wextra -g

INCL = -I incl

LIBHERMES = ./libhermes

SRC_DIR = src
SRC_FILES = main.c worker_daemon.c worker_loop.c sanity_check.c exclude.c
SRC_FILES += manager.c partition.c binnify.c send_work.c message.c

THELP_DIR = src/type_helper
THELP_FILES = ip4.c port.c job.c worker.c

PARSE_DIR = src/parsing
PARSE_FILES = h_custom_payload.c h_files.c h_performace_opts.c h_portlists.c
PARSE_FILES += h_host_discovery.c h_scan.c h_timeouts.c h_worker.c
PARSE_FILES += h_output_ops.c parse_file.c
PARSE_FILES += parse_ip.c parse_ports.c parse_time.c parser.c

SRC = $(addprefix $(SRC_DIR)/, $(SRC_FILES))
SRC += $(addprefix $(PARSE_DIR)/, $(PARSE_FILES))
SRC += $(addprefix $(THELP_DIR)/, $(THELP_FILES))

RM = rm -fr

$(NAME):
		$(MAKE) -C binn
		$(MAKE) -C $(LIBHERMES)
		$(CC) $(CFLAGS) $(INCL) $(SRC) binn/binn.o $(LIBHERMES)/libhermes.a -o $(NAME)

all: $(NAME)

clean:
		$(RM) $(NAME)

fclean: clean
		$(MAKE) -C binn clean
		$(MAKE) -C $(LIBHERMES) fclean

re: fclean $(NAME)