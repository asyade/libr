#!AUTO GENERATED!
SRC=./src/binary_heap.c ./src/rlib.c 
CC=gcc
NAME=libr
OBJ=$(SRC:.c=.o)
INCLUDES=-I ./include 
FLAGS=-Wall -Werror -Wextra -g3

TEST_BHEAP=test/binary_heap.c
TES_BHEAP_BIN=bin/test_bheap

all: $(NAME)

$(NAME): $(OBJ) include/libr.h Makefile
	ar rcs $(NAME) $(OBJ)
	ranlib $(NAME)

%.o : %.c 
	$(CC) $< -c $(INCLUDES) $(LIBS) -o $@ $(FLAGS)

test_bheap: re
	$(CC) $(TEST_BHEAP) $(INCLUDES) $(NAME)  -o $(TES_BHEAP_BIN) $(FLAGS)
	$(TES_BHEAP_BIN)
	

clean:
	rm -f $(OBJ)

fclean: clean
	rm -f $(NAME)

re: fclean all