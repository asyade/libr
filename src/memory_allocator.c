/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   memory_allocator.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acorbeau <acorbeau@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/10/01 17:34:09 by acorbeau          #+#    #+#             */
/*   Updated: 2018/10/07 14:28:46 by acorbeau         ###   ########.fr       */
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
	return join_empty_entries(al, index, tmp) == 0 ? 1 : -4;
}

int				try_join_empty_entry_left(t_memalloc *a, size_t index)
{
	t_mementry	*entry;
	t_memmagic	*magicptr;
	t_memmagic	lm;
	size_t		final;

	entry = EMPTY_PTR(a) + index;
	if ((size_t)entry->addr == ALLOC_SPTR(a))
		return 0;
	lm = *(t_memmagic *)((size_t)entry->addr - sizeof(t_memmagic));
	if ((lm.status != USED && lm.status != FREE) ||
		(size_t)entry->addr - lm.size > ALLOC_SPTR(a) + a->buffer_size ||
		((size_t)entry->addr - ALLOC_SPTR(a)) + entry->size > a->buffer_size)
		return (E_OVERFLOW);
	if (lm.status == USED || (size_t)entry->addr - lm.size == ALLOC_SPTR(a) + a->buffer_size)
		return (0);
	magicptr = (t_memmagic *)((size_t)entry->addr - lm.size);
	if (check_mem_magic(a, (size_t)magicptr - ALLOC_SPTR(a), lm.size, 1) != 0)
		return (E_MAGIC);
	if ((final = bheap_find(a->empty_entries, &(t_mementry){0, magicptr}, 0)) == BH_NOTFOUND)
	{
		ft_putfmt("Cant find empty elem\n");
		memalloc_dump(a);
		return (E_FIND_HEAP);
	}
	return (join_empty_entries(a, final, index) == 0 ? 1 : -4);
}

void			*fill_entry_begin(t_memalloc *allocator, t_mementry entry, size_t size)
{
	size_t		new_index;
	if (entry.size > size && entry.size - size < sizeof(t_memmagic) * 3)
		size = entry.size;
	if (entry.size > size)
	{
		if ((new_index = bheap_insert(allocator->empty_entries, &(t_mementry){entry.size - size, (void *)((size_t)entry.addr + size)})) == BH_NOTFOUND)
		{
			memalloc_seterr(E_INS_EMPTY);
			return (NULL);
		}
		if (fill_mem_magic(allocator, ((size_t)entry.addr - ALLOC_SPTR(allocator)) + size, entry.size - size, FREE, 0) != 0)
		{
			memalloc_seterr(E_MAGIC);
			return (NULL);
		}
	}
	if ((bheap_insert(allocator->used_entries, &(t_mementry){size, entry.addr})) == BH_NOTFOUND)
	{
		memalloc_seterr(E_INS_USED);
		return (NULL);
	}
	if (fill_mem_magic(allocator, (size_t)entry.addr - ALLOC_SPTR(allocator), size, USED, 0) != 0)
	{
		memalloc_seterr(E_MAGIC);
		return (NULL);
	}
	return (entry.addr);
}

void			*fill_entry_middel(t_memalloc *allocator, t_mementry entry, size_t size)
{
	size_t		offset;
	size_t		new_size;
	size_t		new_index;

	offset = (entry.size / 2) - (size / 2);
	new_size = entry.size - (entry.size - offset);
	if (offset % 2)
	{
		offset++;
		new_size = entry.size - (entry.size - offset);
	}
	if ((new_index = bheap_insert(allocator->empty_entries, &(t_mementry){new_size, entry.addr})) == BH_NOTFOUND)
	{
		memalloc_seterr(E_INS_EMPTY);
		return (NULL);
	}
	if (fill_mem_magic(allocator, (size_t)entry.addr - ALLOC_SPTR(allocator), new_size, FREE, 0) != 0)
	{
		memalloc_seterr(E_MAGIC);
		return (NULL);
	}
	return fill_entry_begin(allocator, (t_mementry){entry.size - new_size, (void *)((size_t)entry.addr + new_size)}, size);
}

void			*memalloc_alloc(t_memalloc *allocator, size_t size)
{ //Todo place rwlock here for thread safe
	//Important todo asume that the memmagic use space in size
	size_t		index;
	t_mementry	entry;
	void		*ret;

	size = SIZE_ALIGN((size + (2 * sizeof(t_memmagic))));
	if ((index = find_empty_entry(allocator->empty_entries, size)) == BH_NOTFOUND)
		return (NULL);
	entry = ((t_mementry *)((allocator->empty_entries) + 1))[index];
	if (bheap_remove(allocator->empty_entries, index) != 0)
	{
		memalloc_seterr(E_DEL_HEAP);
		return (NULL);
	}
	if (entry.size >= size * 3)
		ret = (fill_entry_middel(allocator, entry, size));
	else
		ret = (fill_entry_begin(allocator, entry, size));
	return (ret ? (t_memmagic *)ret + 1 : NULL);
}

int				try_join_empty_entries(t_memalloc *allocator, size_t index, void *addr)
{
	int join_result;

	// Try to join with right cell, if it's success index of curent entry may change
	join_result = try_join_empty_entry_right(allocator, index);
	// Find new index of the entry into the heap if is joined to the right cell
	if (join_result > 0 && (index = bheap_find(allocator->empty_entries, &(t_mementry){0, addr}, 0)) == BH_NOTFOUND)
		return (E_FIND_HEAP);
	else if (join_result < 0)
		return (join_result);
	join_result = try_join_empty_entry_left(allocator, index);
	return (join_result >= 0 ? 0 : join_result);
}

int				memalloc_free(t_memalloc *allocator, void *addr)
{
	size_t		index;
	t_mementry	entry;
	int			val;

	if (addr == NULL)
		return (0);
	addr = (t_memmagic *)addr - 1;
	if ((index = bheap_find(allocator->used_entries, &(t_mementry){0, addr}, 0)) == BH_NOTFOUND)
		return (bheap_find(allocator->empty_entries, &(t_mementry){0, addr}, 0) == BH_NOTFOUND ? 1 : E_FIND_HEAP);
	entry = *((t_mementry *)(allocator->used_entries + 1) + index);
	if (bheap_remove(allocator->used_entries, index) != 0)
		return (E_DEL_HEAP);
	if ((index = bheap_insert(allocator->empty_entries, &entry)) == BH_NOTFOUND)
		return (E_INS_EMPTY);
	if (check_mem_magic(allocator, (size_t)addr - ALLOC_SPTR(allocator), entry.size, 1) != 0)
		return (E_MAGIC);
	if (fill_mem_magic(allocator, (size_t)entry.addr - ALLOC_SPTR(allocator), entry.size, FREE, 1) != 0)
		return (E_MAGIC);
	if ((val = try_join_empty_entries(allocator, index, addr)) != 0)
		return (val);
	return (0);
}

int				memalloc_expande_full(t_memalloc *allocator, t_memmagic *self, size_t new_size, size_t indexs[2])
{
	size_t		joined_size;

	joined_size = new_size;
	if (fill_mem_magic(allocator, (size_t)self - ALLOC_SPTR(allocator), joined_size, USED, 1) != 0)
		return (E_MAGIC);
	(USED_PTR(allocator) + indexs[0])->size = joined_size;
	if (bheap_remove(allocator->empty_entries, indexs[1]) != 0)
		return (E_DEL_HEAP);
	return (0);
}

int				memalloc_expande_insert(t_memalloc *allocator, t_memmagic *self, t_memmagic *other, size_t indexs[2], size_t size)
{
	/*   size_t new_size;

	new_size = other->size - (size - self->size);
	if (fill_mem_magic(allocator, (size_t)self + size, new_size, FREE, 0) != 0)
		return (-1);
	if (bheap_insert(allocator->empty_entries, &(t_mementry){new_size, (void *)((size_t)self + new_size)}) == BH_NOTFOUND)
		return (-2);

	return (memalloc_expande_full(allocator, self, other, indexs));
*/
	size++;
	return (memalloc_expande_full(allocator, self, ((size_t)other + other->size) - (size_t)self, indexs));
}

int				memalloc_try_expande(t_memalloc *a, void *addr, size_t ns)
{
	t_memmagic	*s;
	t_memmagic	*o;
	size_t		si;
	size_t		oi;

	if (addr == NULL || ns == 0)
		return (ns == 0 ? 0 : 1);
	ns += 2 * sizeof(t_memmagic);
	s = (t_memmagic *)addr - 1;
	if (check_mem_magic_overflow(a, s) != 0)
		return (E_OVERFLOW);
	o = (t_memmagic *)((size_t)s + s->size);
	if (check_mem_magic_overflow(a, o) != 0)
		return (1);
	if (check_mem_magic(a, (size_t)s - ALLOC_SPTR(a), s->size, 1) != 0 ||
		check_mem_magic(a, (size_t)o - ALLOC_SPTR(a), o->size, 1) != 0)
		return (E_MAGIC);
	if ((si = bheap_find(AUE(a), &PTR_AS_ENTRY(a, s), 0)) == BH_NOTFOUND ||
		(oi = bheap_find(AEE(a), &PTR_AS_ENTRY(a, o), 0)) == BH_NOTFOUND)
		return (1);
	if (s->size >= ns)
		return (0);
	if (s->size + o->size < ns)
		return (1);
	if (s->size + o->size > ns * 2)
		return (memalloc_expande_insert(a, s, o, (size_t[]){si, oi}, ns));
	ns = ((size_t)o + o->size) - (size_t)s;
	return (memalloc_expande_full(a, s, ns, (size_t[]){si, oi}));
}
