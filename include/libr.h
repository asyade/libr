#ifndef LIBR_H
#define LIBR_H

#include <unistd.h>
#include <string.h>
#include <sys/mman.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdarg.h>
#include <stdint.h>

#define CL_RED "\x1b[31m"
#define CL_GREEN "\x1b[32m"
#define CL_YELLOW "\x1b[33m"
#define CL_BLUE "\x1b[34m"
#define CL_MAGENTA "\x1b[35m"
#define CL_CYAN "\x1b[36m"
#define CL_RESET "\x1b[0m"
#define CS_RESET "\033[2J"

/**
 * 	Basic functions
 */
void ft_putchar_buff_fd(char c, int fd);
size_t ft_strlen(char *str);
void ft_putstr(char *str);
void ft_putstr_ft(char *str, int fd);
void ft_putendl(char *str);
void ft_putendl_fd(char *str, int fd);
void ft_putull_fd(uintmax_t nbr, int fd);
void ft_putfmt(char *fmt, ...);

void ft_shift_array(void *array, size_t size, size_t elem_size, long int (*rnd)());
void ft_memswap(unsigned char *src, unsigned char *dest, size_t n);
void *ft_memcpy(void *dest, const void *src, size_t n);
void *ft_memmove(void *dest, const void *src, size_t n);
void ft_memset(unsigned char *dest, unsigned char c, size_t n);
int ft_memcmp(void *a, void *b, size_t n);
/**
 * Simple generic binary heap implementation (only tested as min heap but it's may work as max heap)
 */
#define BH_NOTFOUND ((size_t)-1)
#define BH_PARENT(index) ((((index)-1) / 2))
#define BH_LEFT(index) (((2 * (index)) + 1))
#define BH_RIGHT(index) (((2 * (index)) + 2))
#define BH_INDEX(heap, index) (unsigned char *)(((size_t)((heap) + 1) + ((index) * (heap)->elem_size)))

typedef int (*t_bheap_cmpf)(void *, void *);

typedef struct s_bheap
{
	size_t size;
	size_t capacity;
	size_t elem_size;
	size_t buffer_size;
	t_bheap_cmpf cmpf;
} t_bheap;

void print_heap(t_bheap *heap);
size_t bheap_travers_up(t_bheap *heap, size_t index);
size_t bheap_travers_down(t_bheap *heap, size_t index);
t_bheap *bheap_new(void *buffer, size_t buffer_size, size_t elem_size, t_bheap_cmpf cmpf);
size_t bheap_insert(t_bheap *heap, void *elem);
int bheap_remove(t_bheap *heap, size_t index);
size_t bheap_find(t_bheap *heap, void *value, size_t index);

/**
 * Simple memory chunk alocator for large allocation (use directly mmap)
 */
#define MEMCHUNK_MAGIC 424242
#define MMAP_NULL ((void *)-1)

typedef struct s_memchunk
{
	size_t size;
	size_t magic;
} t_memchunk;

t_memchunk *mchunk_alloc(size_t size);
t_memchunk *mchunk_realloc(t_memchunk *chunk, size_t new_size);
int mchunk_free(t_memchunk *chunk);

/**
 * Optimised memory allocator for random size allocation
*/

#define E_MAP_UNDEF -15
#define E_EXPAND_HEAP -14
#define E_FIND_HEAP -13
#define E_DEL_HEAP -12
#define E_INS_EMPTY -11
#define E_INS_USED -10
#define E_NOMEM -9
#define E_NOFOUND -8
#define E_MAGIC -7
#define E_OVERFLOW -5
#define E_UNDEF -1
#define E_DOUBLEFREE -16

#define MAX_ALLOC_SIZE 1024 * 1024 * 18
#define MIN_ALLOC_SIZE 2
#define ALIGN 8
#define ALLOCATOR_MAX(allocator) ((size_t)(((allocator) + sizeof(t_memalloc) + (allocator)->buffer_size)))
#define SIZE_ALIGN(size) ((((size) / ALIGN) + ((size) % ALIGN ? 1 : 0)) * ALIGN)
#define SIZE_ALLOC(size) (SIZE_ALIGN((size + sizeof(t_alloc))))

#define PTR_AS_ENTRY(allocator, ptr) ((t_mementry){0, (void *)((size_t)(ptr))})

#define ALLOC_VPTR(allocator) (((void *)((allocator) + 1)))
#define ALLOC_SPTR(allocator) (((size_t)((allocator) + 1)))

#define EMPTY_PTR(allocator) (((t_mementry *)((allocator)->emptyEntries + 1)))
#define USED_PTR(allocator) (((t_mementry *)((allocator)->usedEntries + 1)))

typedef enum e_alloc_stat
{
	USED = 0x01,
	FREE = 0x02,
} t_alloc_stat;

//Warning no we need to have no whole on this struct
#pragma pack(1)
typedef struct s_memmagic
{
	t_alloc_stat status;
	size_t size;
} t_memmagic;

typedef struct s_mementry
{
	size_t size;
	void *addr;
} t_mementry;

typedef struct s_szrange
{
	size_t min;
	size_t max;
} t_szrange;

typedef struct s_memalloc
{
	t_szrange range;
	t_bheap *emptyEntries;
	t_bheap *usedEntries;
	size_t buffer_size;
} t_memalloc;
/// IMportant todo replace all bheap_insert by bheap insert or expande
/// Important imporovement can be find element by it's size instead of address in heap
int memalloc_seterr(int code);
int memalloc_geterr();

int entries_cmp(void *aa, void *bb);
void memalloc_panic(int code);
size_t find_empty_entry(t_bheap *heap, size_t size);

int check_mem_magic_overflow(t_memalloc *allocator, t_memmagic *magic);
int check_mem_magic(t_memalloc *allocator, size_t offset, size_t size, int recursive);

int fill_mem_magic(t_memalloc *allocator, size_t offset, size_t size, t_alloc_stat status, int check_recursive);
void memalloc_dump(t_memalloc *allocator);
void print_heap(t_bheap *heap);

t_memalloc *memalloc_new(size_t buffer_size, size_t heap_size, t_szrange range);
void memalloc_destroy(t_memalloc *allocator);

void *memalloc_alloc(t_memalloc *allocator, size_t size);
int memalloc_try_expande(t_memalloc *allocator, void *addr, size_t new_size);
int memalloc_free(t_memalloc *allocator, void *addr);

/**
 * overflay for memory allocator that add thread safety and error handling 
 * It's also expande the allocator heap if need instead of fail operations
 */

#define LOCK_USED 1
#define LOCK_USABLE 0
#define LOCK_GET 2
#define LOCK_LIBERATE 3

void *safe_memalloc_alloc(t_memalloc *allocator, size_t size, int retry);
int safe_memalloc_free(t_memalloc *allocator, void *ptr);
int safe_memalloc_expande(t_memalloc *allocator, void *ptr, size_t new_size);

/*
manage differts memory allocator (using safe overlay)
*/

void *mmemalloc_alloc(size_t size);
void mmemalloc_free(void *ptr);
int mmemalloc_expande(void *ptr, size_t new_size);

#endif