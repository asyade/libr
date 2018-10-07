/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   memory_allocator_utils.c                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acorbeau <acorbeau@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/10/07 16:24:31 by acorbeau          #+#    #+#             */
/*   Updated: 2018/10/07 16:45:08 by acorbeau         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libr.h"

size_t			get_entries_size_sum(t_mementry *entries, size_t size)
{
	size_t		ret;

	ret = 0;
	while (size--)
	{
		ret += entries[size].size;
	}
	return (ret);
}

int				check_mem_magic_overflow(t_memalloc *al, t_memmagic *magic)
{
	if ((size_t)magic + sizeof(t_memmagic) >=
			ALLOC_SPTR(al) + al->buffer_size)
		return (-1);
	if ((size_t)magic + magic->size > ALLOC_SPTR(al) + al->buffer_size)
		return (-2);
	return (0);
}

int				check_mem_magic(t_memalloc *a, size_t o, size_t size, int r)
{
	t_memmagic	magics[2];

	if (size == 0 || size + o > a->buffer_size ||
			o > MAX_ALLOC_SIZE || size > MAX_ALLOC_SIZE)
		return (-1);
	magics[0] = *(t_memmagic *)(ALLOC_SPTR(a) + o);
	magics[1] = *(t_memmagic *)(ALLOC_SPTR(a) + o + (size - STMM));
	if (o % 2 || (ft_memcmp(magics, magics + 1, STMM) != 0 ||
			magics[0].size != size || !(magics[0].status & (USED | FREE))))
		return (-2);
	if (!r)
		return (0);
	if (o > 0)
	{
		magics[0] = *(t_memmagic *)(ALLOC_SPTR(a) + (o - STMM));
		if (check_mem_magic(a, o - magics[0].size, magics[0].size, 0) != 0)
			return (-3);
	}
	if (o + size < a->buffer_size)
	{
		magics[0] = *(t_memmagic *)(ALLOC_SPTR(a) + o + size);
		if (check_mem_magic(a, o + size, magics[0].size, 0) != 0)
			return (-4);
	}
	return (0);
}

size_t			find_empty_entry(t_bheap *heap, size_t size)
{
	size_t		i;
	size_t		choice;
	t_mementry	*entries;

	choice = BH_NOTFOUND;
	entries = (t_mementry *)(heap + 1);
	i = 0;
	while (i < heap->size)
	{
		if (entries[i].size >= size * 3)
		{
			choice = i;
			break ;
		}
		else if (entries[i].size >= size)
			choice = i;
		i++;
	}
	return (choice);
}
