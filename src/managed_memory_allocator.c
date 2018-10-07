/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   managed_memory_allocator.c                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acorbeau <acorbeau@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/10/01 16:34:12 by acorbeau          #+#    #+#             */
/*   Updated: 2018/10/01 18:09:14 by acorbeau         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libr.h"

int					small_page_count(int delta)
{
	static int		nbr = 0;

	nbr += delta;
	return (nbr);
}

int					big_page_count(int delta)
{
	static int		nbr = 0;

	nbr += delta;
	return (nbr);
}

int					memallocator_cmpf(void *aa, void *bb)
{
	size_t			a;
	size_t			b;

	a = (size_t)(*(void **)aa);
	b = (size_t)(*(void **)bb);
	if (a == b)
		return (0);
	return (a < b) ? -1 : 1;
}

t_bheap				*mmemalloc_heap(void)
{
	static t_bheap	*heap = NULL;
	t_memchunk		*c;

	if (heap == NULL)
	{
		if ((c = mchunk_alloc(ALLOCATORS_HEAP_SIZE)) == NULL)
			memalloc_panic(E_NOMEM);
		heap = bheap_new(c + 1, c->size, sizeof(void *), memallocator_cmpf);
	}
	return (heap);
}

void				*find_and_alloc(size_t size, size_t index)
{
	t_bheap			*heap;
	t_memalloc		*node;
	void			*ptr;

	heap = mmemalloc_heap();
	if (index >= heap->size)
		return (NULL);
	node = (t_memalloc *)(heap + 1) + index;
	if (node->range.min < size && node->range.max > size &&
		(ptr = safe_memalloc_alloc(node, size, 1)) != NULL)
		return (ptr);
	if ((ptr = find_and_alloc(size, BH_LEFT(index))) != NULL)
		return (ptr);
	if ((ptr = find_and_alloc(size, BH_RIGHT(index))) != NULL)
		return (ptr);
	return (NULL);
}

t_memalloc			*memalloc_new_range(size_t range)
{
	if (range < 128)
	{
		small_page_count(1);
		return (memalloc_new(SMALL_SZ, 4096, (t_szrange){SM_MIN, SM_MAX}));
	}
	if (range < 1024)
	{
		big_page_count(1);
		return (memalloc_new(BIG_SZ, 4096, (t_szrange){BG_MIN, BG_MAX}));
	}
	return (memalloc_new(range, 1024, (t_szrange){-1, -1}));
}

void				*insert_and_alloc(size_t range)
{
	t_memalloc		*allocator;

	if (mmemalloc_heap()->size >= MAX_ALLOC_PAGES)
		return (NULL);
	if ((allocator = memalloc_new_range(range)) == NULL)
		return (NULL);
	if (bheap_insert(mmemalloc_heap(), &allocator) == BH_NOTFOUND)
		memalloc_panic(E_NOMEM);
	return (safe_memalloc_alloc(allocator, range, 0));
}

void				*mmemalloc_alloc(size_t size)
{
	void			*ptr;

	if (size > MAX_ALLOC_SIZE)
		return (NULL);
	if ((ptr = find_and_alloc(size, 0)) != NULL)
		return (ptr);
	return (insert_and_alloc(size));
}

t_memalloc			*find_allocator_by_addr(void *ptr, size_t index)
{
	t_bheap			*heap;
	t_memalloc		*al;

	heap = mmemalloc_heap();
	if (index >= heap->size)
		return (NULL);
	al = *((t_memalloc **)(heap + 1) + index);
	if ((size_t)al < (size_t)ptr &&
		(size_t)ptr < (size_t)(al + 1) + al->buffer_size)
		return (al);
	if ((al = find_allocator_by_addr(ptr, BH_LEFT(index))) != NULL)
		return (al);
	if ((al = find_allocator_by_addr(ptr, BH_RIGHT(index))) != NULL)
		return (al);
	return (NULL);
}

void				mmemalloc_free_big(t_bheap *heap, t_memalloc *allocator)
{
	heap = mmemalloc_heap();
	if (bheap_remove(heap, bheap_find(heap, &allocator, 0)) != 0)
		memalloc_panic(E_UNDEF);
	memalloc_destroy(allocator);
}

void				mmemalloc_free(void *ptr)
{
	t_memalloc		*allocator;
	t_bheap			*heap;

	heap = mmemalloc_heap();
	if ((allocator = find_allocator_by_addr(ptr, 0)) == NULL)
		memalloc_panic(E_OVERFLOW);
	if (allocator->range.min == (size_t)-1)
		mmemalloc_free_big(heap, allocator);
	else if (memalloc_free(allocator, ptr) < 0)
		memalloc_panic(E_OVERFLOW);
	else if (allocator->used_entries->size != 0)
		return ;
	else if ((allocator->range.min == SM_MIN && small_page_count(0) > 1))
	{
		bheap_remove(heap, bheap_find(heap, &allocator, 0));
		memalloc_destroy(allocator);
		small_page_count(-1);
	}
	else if (allocator->range.min == BG_MIN && big_page_count(0) > 1)
	{
		bheap_remove(heap, bheap_find(heap, &allocator, 0));
		memalloc_destroy(allocator);
		big_page_count(-1);
	}
}

int					mmemalloc_expande(void *ptr, size_t new_size)
{
	t_memalloc	*allocator;

	if (new_size > MAX_ALLOC_SIZE)
		return (1);
	if ((allocator = find_allocator_by_addr(ptr, 0)) == NULL)
		memalloc_panic(E_OVERFLOW);
	return (memalloc_try_expande(allocator, ptr, new_size));
}
