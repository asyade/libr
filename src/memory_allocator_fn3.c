/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   memory_allocator_fn3.c                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acorbeau <acorbeau@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/10/07 17:00:20 by acorbeau          #+#    #+#             */
/*   Updated: 2018/10/07 17:01:21 by acorbeau         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libr.h"

int				memalloc_expande_full(t_memalloc *a,t_memmagic *s, size_t ns, size_t i[2])
{
	size_t		joined_size;

	joined_size = ns;
	if (fill_mem_magic(a, (size_t)s - ALLOC_SPTR(a), joined_size, USED, 1) != 0)
		return (E_MAGIC);
	(USED_PTR(a) + i[0])->size = joined_size;
	if (bheap_remove(a->empty_entries, i[1]) != 0)
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
