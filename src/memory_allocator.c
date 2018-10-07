/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   memory_allocator.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acorbeau <acorbeau@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/10/01 17:34:09 by acorbeau          #+#    #+#             */
/*   Updated: 2018/10/07 17:00:51 by acorbeau         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libr.h"

void			*fill_entry_begin(t_memalloc *a, t_mementry e, size_t s)
{
	size_t		ne;

	if (e.size > s && e.size - s < sizeof(t_memmagic) * 3)
		s = e.size;
	if (e.size > s)
	{
		if ((ne = bheap_insert(a->empty_entries, &(t_mementry){
				e.size - s, (void *)((size_t)e.addr + s)})) == BH_NOTFOUND)
			return (memalloc_seterr(E_INS_EMPTY) == 0 ? NULL : NULL);
		if (fill_mem_magic(a, ((size_t)e.addr - ALLOC_SPTR(a)) + s,
				e.size - s, FREE, 0) != 0)
			return (memalloc_seterr(E_MAGIC) == 0 ? NULL : NULL);
	}
	if ((bheap_insert(a->used_entries,
			&(t_mementry){s, e.addr})) == BH_NOTFOUND)
		return (memalloc_seterr(E_INS_USED) == 0 ? NULL : NULL);
	if (fill_mem_magic(a, (size_t)e.addr - ALLOC_SPTR(a), s, USED, 0) != 0)
		return (memalloc_seterr(E_MAGIC) == 0 ? NULL : NULL);
	return (e.addr);
}

void			*fill_entry_middel(t_memalloc *a, t_mementry e, size_t size)
{
	size_t		offset;
	size_t		new_size;
	size_t		new_index;

	offset = (e.size / 2) - (size / 2);
	new_size = e.size - (e.size - offset);
	if (offset % 2)
	{
		offset++;
		new_size = e.size - (e.size - offset);
	}
	if ((new_index = bheap_insert(a->empty_entries,
			&(t_mementry){new_size, e.addr})) == BH_NOTFOUND)
	{
		memalloc_seterr(E_INS_EMPTY);
		return (NULL);
	}
	if (fill_mem_magic(a, (size_t)e.addr - ALLOC_SPTR(a),
		new_size, FREE, 0) != 0)
	{
		memalloc_seterr(E_MAGIC);
		return (NULL);
	}
	return (fill_entry_begin(a, (t_mementry){e.size - new_size,
							(void *)((size_t)e.addr + new_size)}, size));
}

void			*memalloc_alloc(t_memalloc *a, size_t size)
{
	size_t		index;
	t_mementry	entry;
	void		*ret;

	size = SIZE_ALIGN((size + (2 * sizeof(t_memmagic))));
	if ((index = find_empty_entry(a->empty_entries, size)) == BH_NOTFOUND)
		return (NULL);
	entry = ((t_mementry *)((a->empty_entries) + 1))[index];
	if (bheap_remove(a->empty_entries, index) != 0)
	{
		memalloc_seterr(E_DEL_HEAP);
		return (NULL);
	}
	if (entry.size >= size * 3)
		ret = (fill_entry_middel(a, entry, size));
	else
		ret = (fill_entry_begin(a, entry, size));
	return (ret ? (t_memmagic *)ret + 1 : NULL);
}

int				memalloc_free(t_memalloc *a, void *addr)
{
	size_t		index;
	t_mementry	entry;
	int			val;

	if (addr == NULL)
		return (0);
	addr = (t_memmagic *)addr - 1;
	if ((index = bheap_find(a->used_entries,
		&(t_mementry){0, addr}, 0)) == BH_NOTFOUND)
		return (1);
	entry = *((t_mementry *)(a->used_entries + 1) + index);
	if (bheap_remove(a->used_entries, index) != 0)
		return (E_DEL_HEAP);
	if ((index = bheap_insert(a->empty_entries, &entry)) == BH_NOTFOUND)
		return (E_INS_EMPTY);
	if (check_mem_magic(a, (size_t)addr - ALLOC_SPTR(a), entry.size, 1) != 0)
		return (E_MAGIC);
	if (fill_mem_magic(a, (size_t)entry.addr -
			ALLOC_SPTR(a), entry.size, FREE, 1) != 0)
		return (E_MAGIC);
	if ((val = try_join_empty_entries(a, index, addr)) != 0)
		return (val);
	return (0);
}
