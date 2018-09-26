#ifndef LIBR_H
#define LIBR_H

#include <unistd.h>
#include <string.h>
#include <sys/mman.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

#define MMAP_NULL ((void *)-1)

void ft_shift_array(unsigned char *array, size_t size, size_t elem_size, int (*rnd)());
void ft_memswap(unsigned char *src, unsigned char *dest, size_t n);
void *ft_memcpy(void *dest, const void *src, size_t n);
void ft_memset(unsigned char *dest, unsigned char c, size_t n);

/**
 * Simple generic binary heap implementation (only tested as min heap but it's may work as max heap)
 */
typedef int (*t_bheap_cmpf)(void *, void *);

typedef struct s_bheap
{
	size_t size;
	size_t capacity;
	size_t elem_size;
	size_t buffer_size;
	t_bheap_cmpf cmpf;
} t_bheap;

#define BH_NOTFOUND ((size_t)-1)
#define BH_PARENT(index) ((((index)-1) / 2))
#define BH_LEFT(index) (((2 * (index)) + 1))
#define BH_RIGHT(index) (((2 * (index)) + 2))
#define BH_INDEX(heap, index) (unsigned char *)(((size_t)((heap) + 1) + ((index) * (heap)->elem_size)))

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

typedef enum e_alloc_stat
{
	USED,
	FREE,
} t_alloc_stat;

typedef int t_memsign;

typedef struct s_alloc
{
	size_t size;
	size_t user_size;
	t_alloc_stat status;
	t_memsign sign;
} t_alloc;

typedef struct s_memalloc
{
	struct s_memalloc *next;
	t_alloc *buffer;
} t_memalloc;

#define ALIGN 8
#define SIZE_ALIGN(size) ((((size) / ALIGN) + ((size) % ALIGN ? 1 : 0)) * ALIGN)
#define SIZE_ALLOC(size) (SIZE_ALIGN((size + sizeof(t_alloc))))

t_memalloc *memalloc_new(size_t buffer_size);
void *memalloc_get_chunk(t_memalloc *allocator, size_t size);

#endif