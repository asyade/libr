#ifndef LIBR_H
#define LIBR_H

#include <unistd.h>
#include <string.h>
#include <sys/mman.h>
#include <limits.h>
#include <stdio.h>

/**
 *  capacity: Number of elem_size reaming in alloc_size - sizeof(t_bheap)
 */
typedef int (*t_bheap_cmpf)(void *, void *);

typedef struct s_bheap
{
	size_t size;
	size_t capacity;
	size_t elem_size;
	size_t alloc_size;
	t_bheap_cmpf cmpf;
} t_bheap;

#define BH_PARENT(index) ((((index)-1) / 2))
#define BH_LEFT(index) (((2 * (index)) + 1))
#define BH_RIGHT(index) (((2 * (index)) + 2))

int check_heap(t_bheap *heap, int index);
void ft_shift_array(unsigned char *array, size_t size, size_t elem_size, int (*rnd)());
void ft_memswap(unsigned char *src, unsigned char *dest, size_t n);
void *ft_memcpy(void *dest, const void *src, size_t n);

/**
 * Init new min heap with capacity using mmap and copy minval to the first element of the heap (for optime)
 */
t_bheap *bheap_new(size_t elem_size, size_t capacity, t_bheap_cmpf cmpf);
int bheap_insert(t_bheap *heap, void *elem);
int bheap_remove(t_bheap *heap, size_t index);
void *bh_index(t_bheap *ptr, size_t index);

#endif