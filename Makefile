
NAME = hermes

CC = gcc

SRC_DIR = src

TEST_FLAGS = -Wall -Werror -Wextra -g

CFLAGS  = -Wall -Werror -Wextra -g -fsanitize=address

LIBHERMES = libhermes/

LIBBINN = binn/

UNITSRC_DIR = src/unit_testing
UNITSRC_FILES = munit.c

SRC_DIR = src
SRC_FILES = worker_daemon.c worker_loop.c sanity_check.c exclude.c
SRC_FILES += manager.c partition.c binnify.c message.c thread.c     \
                unbinnify.c

THELP_DIR = src/type_helper
THELP_FILES = ip4.c port.c job.c worker.c mgr.c result.c

PARSE_DIR = src/parsing
PARSE_FILES = h_custom_payload.c h_files.c h_performace_opts.c h_portlists.c
PARSE_FILES += h_host_discovery.c h_scan.c h_timeouts.c h_worker.c
PARSE_FILES += h_output_ops.c parse_file.c
PARSE_FILES += parse_ip.c parse_ports.c parse_time.c parser.c

SRC = $(addprefix $(SRC_DIR)/, $(SRC_FILES))
SRC += $(addprefix $(PARSE_DIR)/, $(PARSE_FILES))
SRC += $(addprefix $(THELP_DIR)/, $(THELP_FILES))
SRC += binn/binn.o
SRC += libhermes/libhermes.a

UNIT_SRC = $(SRC)
UNIT_SRC += munit/munit.c
UNIT_SRC += $(addprefix $(UNITSRC_DIR)/, $(UNITSRC_FILES))

RM = rm -fr

$(NAME):
		$(MAKE) -C $(LIBBINN)
		$(MAKE) -C $(LIBHERMES)
		$(CC) $(CFLAGS) $(SRC) src/main.c -lpthread -o $(NAME)

all: $(NAME)

test:
		$(MAKE) -C $(LIBBINN)
		$(MAKE) -C $(LIBHERMES)
		$(CC) $(TEST_FLAGS) $(UNIT_SRC) -o hermes_test
clean:
		$(RM) $(NAME)

fclean: clean
		$(MAKE) -C $(LIBBINN) clean
		$(MAKE) -C $(LIBHERMES) fclean

re: fclean $(NAME)