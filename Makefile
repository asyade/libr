#!AUTO GENERATED!
SRC=./src/binary_heap.c ./src/rlib.c ./src/memory_chunk.c ./src/memory_allocator.c ./src/memory_allocator_ctor.c ./src/memory_allocator_utils.c ./src/memalloc_errno.c ./src/thread_safe_allocator.c ./src/memalloc_errno.c ./src/thread_safe_allocator.c ./src/managed_memory_allocator.c
CC=gcc
NAME=libr
OBJ=$(SRC:.c=.o)
INCLUDES=-I ./include 
FLAGS= -Wno-unused-result -Wall -Werror -Wextra -g3 -fPIC

TEST_BHEAP=test/binary_heap.c
TES_BHEAP_BIN=bin/test_bheap

TEST_MEM=test/memalloc.c
TES_MEM_BIN=bin/test_mem

all: $(NAME)

$(NAME): $(OBJ) include/libr.h Makefile
	ar rcs $(NAME) $(OBJ)
	ranlib $(NAME)

%.o : %.c 
	$(CC) $< -c $(INCLUDES) $(LIBS) -o $@ $(FLAGS)

test_bheap: re
	$(CC) $(TEST_BHEAP) $(INCLUDES) $(NAME)  -o $(TES_BHEAP_BIN) $(FLAGS)
	$(TES_BHEAP_BIN)
	
test_mem: re
	$(CC) $(TEST_MEM) $(INCLUDES) $(NAME)  -o $(TES_MEM_BIN) $(FLAGS)
	$(TES_MEM_BIN)
	

clean:
	rm -f $(OBJ)

fclean: clean
	rm -f $(NAME)

re: fclean all