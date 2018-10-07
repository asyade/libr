/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   libr.h                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acorbeau <acorbeau@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/10/01 15:45:18 by acorbeau          #+#    #+#             */
/*   Updated: 2018/10/07 19:16:20 by acorbeau         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LIBR_H
# define LIBR_H

# include <unistd.h>
# include <string.h>
# include <sys/mman.h>
# include <limits.h>
# include <stdio.h>
# include <stdlib.h>
# include <pthread.h>
# include <stdarg.h>
# include <stdint.h>

# define CL_RED "\x1b[31m"
# define CL_GREEN "\x1b[32m"
# define CL_YELLOW "\x1b[33m"
# define CL_BLUE "\x1b[34m"
# define CL_MAGENTA "\x1b[35m"
# define CL_CYAN "\x1b[36m"
# define CL_RESET "\x1b[0m"
# define CS_RESET "\033[2J"

void			ft_putchar_fd(char c, int fd);
void			ft_putchar_buff_fd(char c, int fd);
size_t			ft_strlen(char *str);
void			ft_putstr(char *str);
void			ft_putstr_ft(char *str, int fd);
void			ft_putendl(char *str);
void			ft_putendl_fd(char *str, int fd);
void			ft_putull_fd(uintmax_t nbr, int fd);


# define FMT_BUFFER_SIZE 4096
# define PUTFMT_BUFFER_SIZE 1024 * 16
void			ft_putfmt(char *fmt, ...);

typedef long int(t_rnd)();

void			ft_shift_array(void *a, size_t s, size_t e, t_rnd rnd);
void			ft_memswap(unsigned char *src, unsigned char *dest, size_t n);
void			*ft_memcpy(void *dest, const void *src, size_t n);
void			*ft_memmove(void *dest, const void *src, size_t n);
void			ft_memset(unsigned char *dest, unsigned char c, size_t n);
int				ft_memcmp(void *a, void *b, size_t n);

# define BH_NOTFOUND ((size_t)-1)
# define BHN ((size_t)-1)
# define SV size_t[]
# define MXE memalloc_expande_insert
# define TMA t_memalloc
# define TMM t_memmagic
# define BH_PARENT(index) ((((index)-1) / 2))
# define BH_LEFT(index) (((2 * (index)) + 1))
# define BH_RIGHT(index) (((2 * (index)) + 2))

# define IH(ii, hh)(((ii) * (hh)->elem_size))
# define BH_INDEX(hh, ii) (uint8_t *)(((size_t)((hh) + 1) + IH(ii, hh)))

typedef int		(*t_bheap_cmpf)(void *, void *);

# define BHCMPF t_bheap_cmpf

typedef struct	s_bheap
{
	size_t		size;
	size_t		capacity;
	size_t		elem_size;
	size_t		buffer_size;
	BHCMPF		cmpf;
}				t_bheap;

size_t			bheap_heapify(t_bheap *h, size_t i);
void			ft_putnbrbase_fd(uintmax_t nbr, char *base, int fd);
void			print_heap(t_bheap *heap);
size_t			bheap_travers_up(t_bheap *heap, size_t index);
size_t			bheap_travers_down(t_bheap *heap, size_t index);
t_bheap			*bheap_new(void *b, size_t bs, size_t es, t_bheap_cmpf c);
size_t			bheap_insert(t_bheap *heap, void *elem);
int				bheap_remove(t_bheap *heap, size_t index);
size_t			bheap_find(t_bheap *heap, void *value, size_t index);

# define MEMCHUNK_MAGIC 424242
# define MMAP_NULL ((void *)-1)
# define MMAP_FLAGS MAP_ANONYMOUS | MAP_PRIVATE
# define MMAP_PROT PROT_WRITE | PROT_READ

typedef struct	s_memchunk
{
	size_t		size;
	size_t		magic;
}				t_memchunk;

t_memchunk		*mchunk_alloc(size_t size);
t_memchunk		*mchunk_realloc(t_memchunk *chunk, size_t new_size);
int				mchunk_free(t_memchunk *chunk);

# define E_MAP_UNDEF -15
# define E_EXPAND_HEAP -14
# define E_FIND_HEAP -13
# define E_DEL_HEAP -12
# define E_INS_EMPTY -11
# define E_INS_USED -10
# define E_NOMEM -9
# define E_NOFOUND -8
# define E_MAGIC -7
# define E_OVERFLOW -5
# define E_UNDEF -1
# define E_DOUBLEFREE -16
# define MAX_ALLOC_SIZE 1024 * 1024 * 18
# define MIN_ALLOC_SIZE 2
# define ALIGN 8
# define ABZ(a)((a)->buffer_size)
# define ALLOCATOR_MAX(a) ((size_t)(((a) + sizeof(t_memalloc) + ABZ(a))))
# define SIZE_ALIGN(s) ((((s) / ALIGN) + ((s) % ALIGN ? 1 : 0)) * ALIGN)
# define SIZE_ALLOC(size) (SIZE_ALIGN((size + sizeof(t_alloc))))
# define PTR_AS_ENTRY(allocator, ptr) ((t_mementry){0, (void *)((size_t)(ptr))})
# define ALLOC_VPTR(allocator) (((void *)((allocator) + 1)))
# define ALLOC_SPTR(allocator) (((size_t)((allocator) + 1)))
# define EMPTY_PTR(allocator) (((t_mementry *)((allocator)->empty_entries + 1)))
# define USED_PTR(allocator) (((t_mementry *)((allocator)->used_entries + 1)))
# define CMMM check_mem_magic

typedef enum	e_alloc_stat
{
	USED = 0x01,
	FREE = 0x02,
}				t_alloc_stat;

# define TALS t_alloc_stat

# pragma pack(1)

typedef struct	s_memmagic
{
	TALS		status;
	size_t		size;
}				t_memmagic;

typedef struct	s_mementry
{
	size_t		size;
	void		*addr;
}				t_mementry;

typedef struct	s_szrange
{
	size_t		min;
	size_t		max;
}				t_szrange;

typedef struct	s_memalloc
{
	t_szrange	range;
	t_bheap		*empty_entries;
	t_bheap		*used_entries;
	size_t		buffer_size;
}				t_memalloc;

int				memalloc_seterr(int code);
int				memalloc_geterr();

int				entries_cmp(void *aa, void *bb);
void			memalloc_panic(int code);
size_t			find_empty_entry(t_bheap *heap, size_t size);

# define TMML t_memalloc

int				check_mem_magic_overflow(TMML *a, t_memmagic *m);
int				check_mem_magic(TMML *allocator, size_t o, size_t s, int r);
int				fill_mem_magic(TMML *a, size_t o, size_t s, TALS ss, int r);
t_memalloc		*memalloc_new(size_t bs, size_t hs, t_szrange range);
void			memalloc_destroy(t_memalloc *allocator);
void			*memalloc_alloc(t_memalloc *allocator, size_t size);
int				memalloc_try_expande(t_memalloc *al, void *addr, size_t ns);
int				memalloc_free(t_memalloc *allocator, void *addr);
int				try_join_empty_entries(t_memalloc *a, size_t index, void *d);

# define PLUSA size_t of, size_t sz
# define LOCK_USED 1
# define LOCK_USABLE 0
# define LOCK_GET 2
# define LOCK_LIBERATE 3
# define MAX_ALLOC_PAGES 1024 * 128
# define MC1(hp, sm) ((size_t)(sm) - (size_t)((hp) + 1))
# define ALLOCATORS_HEAP_SIZE 1024 * 256 * sizeof(void *)
# define SM_MIN 0
# define SM_MAX 128
# define BG_MIN 128
# define BG_MAX 1024
# define SMALL_SZ (1024 * 1024)
# define BIG_SZ (1024 * 1024 * 8)
# define AUE(a) a->used_entries
# define AEE(a) a->empty_entries
# define STM sizeof(t_mementry)
# define STMM sizeof(t_memmagic)

t_bheap			*mmemalloc_heap(void);
int				memallocator_cmpf(void *aa, void *bb);
int				big_page_count(int delta);
int				small_page_count(int delta);

void			*safe_memalloc_alloc(t_memalloc *a, size_t s, int r);
int				safe_memalloc_free(t_memalloc *allocator, void *ptr);
int				safe_memalloc_expande(t_memalloc *a, void *p, size_t n);
void			*mmemalloc_alloc(size_t size);
void			mmemalloc_free(void *ptr);
int				mmemalloc_expande(void *ptr, size_t new_size);

#endif
