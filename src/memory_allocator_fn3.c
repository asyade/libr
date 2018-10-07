/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   memory_allocator_fn3.c                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acorbeau <acorbeau@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/10/07 17:00:20 by acorbeau          #+#    #+#             */
/*   Updated: 2018/10/07 19:16:25 by acorbeau         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libr.h"

int				memalloc_expande_full(TMA *a, TMM *s, size_t ns, size_t i[2])
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

int				memalloc_expande_insert(t_memalloc *a, t_memmagic *s, t_memmagic *o, size_t i[3])
{
	/*   size_t new_size;
	** let size = i[2];
	** new_size = o->size - (size - s->size);
	** if (fill_mem_magic(a, (size_t)s + size, new_size, FREE, 0) != 0)
	** 	return (-1);
	** if (bheap_insert(a->empty_entries, &(t_mementry){new_size, (void *)((size_t)s + new_size)}) == BH_NOTFOUND)
	** 	return (-2);
	** 
	** return (memalloc_expande_full(a, s, o, i));
	*/
	return (memalloc_expande_full(a, s, ((size_t)o + o->size) - (size_t)s, i));
}

int				memalloc_try_expande(t_memalloc *a, void *addr, size_t ns)
{
	t_memmagic	*so[2];
	size_t		ss[2];

	if (addr == NULL || ns == 0)
		return (ns == 0 ? 0 : 1);
	ns += 2 * sizeof(t_memmagic);
	so[0] = (t_memmagic *)addr - 1;
	if (check_mem_magic_overflow(a, so[0]) != 0)
		return (E_OVERFLOW);
	so[1] = (t_memmagic *)((size_t)so[0] + so[0]->size);
	if (check_mem_magic_overflow(a, so[1]) != 0)
		return (1);
	if (CMMM(a, (size_t)so[0] - ALLOC_SPTR(a), so[0]->size, 1) != 0 ||
		CMMM(a, (size_t)so[1] - ALLOC_SPTR(a), so[1]->size, 1) != 0)
		return (E_MAGIC);
	if ((ss[0] = bheap_find(AUE(a), &PTR_AS_ENTRY(a, so[0]), 0)) == BHN ||
		(ss[1] = bheap_find(AEE(a), &PTR_AS_ENTRY(a, so[1]), 0)) == BHN)
		return (1);
	if (so[0]->size >= ns || so[0]->size + so[1]->size < ns)
		return (so[0]->size >= ns ? 0 : 1);
	if (so[0]->size + so[1]->size > ns * 2)
		return (MXE(a, so[0], so[1], (SV){ss[0], ss[1], ns}));
	ns = ((size_t)so[1] + so[1]->size) - (size_t)so[0];
	return (memalloc_expande_full(a, so[0], ns, (size_t[]){ss[0], ss[1]}));
}
