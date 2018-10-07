/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   managed_memory_allocator.c                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acorbeau <acorbeau@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/10/01 16:34:12 by acorbeau          #+#    #+#             */
/*   Updated: 2018/10/07 18:28:43 by acorbeau         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libr.h"

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
