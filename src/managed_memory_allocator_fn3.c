/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   managed_memory_allocator_fn3.c                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acorbeau <acorbeau@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/10/07 18:23:57 by acorbeau          #+#    #+#             */
/*   Updated: 2018/10/07 18:27:31 by acorbeau         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libr.h"

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
