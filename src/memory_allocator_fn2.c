/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   memory_allocator_fn2.c                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acorbeau <acorbeau@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/10/07 16:56:47 by acorbeau          #+#    #+#             */
/*   Updated: 2018/10/07 18:29:01 by acorbeau         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libr.h"

int				fill_mem_magic(t_memalloc *a, PLUSA, t_alloc_stat s, int cr)
{
	t_memmagic *magic;

	if (a->buffer_size < of + sz)
	{
		ft_putfmt("Invalide memory arena %u - %u for buffer size of %u\n",
					of, of + sz, a->buffer_size);
		return (1);
	}
	if (of > (size_t)a)
		return (E_OVERFLOW);
	magic = (t_memmagic *)(ALLOC_SPTR(a) + of);
	*magic = (t_memmagic){s, sz};
	magic = (t_memmagic *)((ALLOC_SPTR(a) + of + sz) - sizeof(t_memmagic));
	*magic = (t_memmagic){s, sz};
	return (check_mem_magic(a, of, sz, cr));
}

int				join_empty_entries(t_memalloc *a, size_t f, size_t drained)
{
	t_mementry	drained_entry;
	t_mementry	*f_entry;
	size_t		off;

	if (f == BH_NOTFOUND || drained == BH_NOTFOUND)
		return (E_FIND_HEAP);
	drained_entry = *(EMPTY_PTR(a) + drained);
	f_entry = (EMPTY_PTR(a) + f);
	f_entry->size += drained_entry.size;
	off = (size_t)f_entry->addr - ALLOC_SPTR(a);
	if (fill_mem_magic(a, off, f_entry->size, FREE, 1) != 0)
		return (E_MAGIC);
	if (bheap_remove(a->empty_entries, drained) != 0)
		return (E_DEL_HEAP);
	return (0);
}

int				try_join_empty_entry_right(t_memalloc *al, size_t index)
{
	t_memmagic	*magicptr;
	t_mementry	*entry;
	size_t		tmp;

	entry = EMPTY_PTR(al) + index;
	magicptr = (t_memmagic *)((size_t)entry->addr + entry->size);
	tmp = (size_t)al + sizeof(t_memalloc) + al->buffer_size;
	if ((size_t)magicptr + sizeof(t_memmagic) > tmp)
		return (0);
	tmp = ((size_t)entry->addr + entry->size) - ALLOC_SPTR(al);
	if (check_mem_magic(al, tmp, magicptr->size, 1) != 0)
		return (E_MAGIC);
	if (magicptr->status == USED)
		return (0);
	tmp = bheap_find(al->empty_entries, &(t_mementry){0, magicptr}, 0);
	return (join_empty_entries(al, index, tmp) == 0 ? 1 : -4);
}

int				try_join_empty_entry_left(t_memalloc *a, size_t index)
{
	t_mementry	*entry;
	t_memmagic	*magicptr;
	t_memmagic	lm;
	size_t		final;

	entry = EMPTY_PTR(a) + index;
	if ((size_t)entry->addr == ALLOC_SPTR(a))
		return (0);
	lm = *(t_memmagic *)((size_t)entry->addr - sizeof(t_memmagic));
	if ((lm.status != USED && lm.status != FREE) ||
		(size_t)entry->addr - lm.size > ALLOC_SPTR(a) + a->buffer_size ||
		((size_t)entry->addr - ALLOC_SPTR(a)) + entry->size > a->buffer_size)
		return (E_OVERFLOW);
	if (lm.status == USED ||
		(size_t)entry->addr - lm.size == ALLOC_SPTR(a) + a->buffer_size)
		return (0);
	magicptr = (t_memmagic *)((size_t)entry->addr - lm.size);
	if (check_mem_magic(a, (size_t)magicptr - ALLOC_SPTR(a), lm.size, 1) != 0)
		return (E_MAGIC);
	if ((final = bheap_find(a->empty_entries,
			&(t_mementry){0, magicptr}, 0)) == BH_NOTFOUND)
		return (E_FIND_HEAP);
	return (join_empty_entries(a, final, index) == 0 ? 1 : -4);
}

int				try_join_empty_entries(t_memalloc *a, size_t index, void *d)
{
	int join_result;

	join_result = try_join_empty_entry_right(a, index);
	if (join_result > 0 &&
			(index = bheap_find(a->empty_entries,
			&(t_mementry){0, d}, 0)) == BH_NOTFOUND)
		return (E_FIND_HEAP);
	else if (join_result < 0)
		return (join_result);
	join_result = try_join_empty_entry_left(a, index);
	return (join_result >= 0 ? 0 : join_result);
}
