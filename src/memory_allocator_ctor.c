/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   memory_allocator_ctor.c                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acorbeau <acorbeau@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/10/01 16:53:03 by acorbeau          #+#    #+#             */
/*   Updated: 2018/10/01 17:33:20 by acorbeau         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libr.h"

int				entries_cmp(void *aa, void *bb)
{
	t_mementry	*a;
	t_mementry	*b;

	a = (t_mementry *)aa;
	b = (t_mementry *)bb;
	if (a->addr == b->addr)
		return (0);
	return (a->addr < b->addr ? -1 : 1);
}

t_memalloc		*memalloc_new(size_t bs, size_t hs, t_szrange r)
{
	t_memalloc	*a;
	t_memchunk	*c;
	size_t		e;
	size_t		ss;

	if ((c = mchunk_alloc(bs)) == NULL)
		return (NULL);
	a = (t_memalloc *)(c + 1);
	a->buffer_size = c->size - sizeof(t_memalloc);
	if ((c = mchunk_alloc(hs)) == NULL)
	{
		mchunk_free((t_memchunk *)(a - 1) - 1);
		return (NULL);
	}
	e = c->size / 2;
	ss = c->size / 2;
	a->range = r;
	AEE(a) = bheap_new(c + 1, e, sizeof(t_mementry), entries_cmp);
	AUE(a) = bheap_new((void *)((size_t)(AEE(a)) + e), ss, STM, entries_cmp);
	bheap_insert(a->empty_entries, &(t_mementry){a->buffer_size, a + 1});
	fill_mem_magic(a, 0, a->buffer_size, FREE, 1);
	return (a);
}

void			memalloc_destroy(t_memalloc *a)
{
	if (!a)
		return ;
	if (mchunk_free((t_memchunk *)((size_t)AEE(a) - sizeof(t_memchunk))) != 0 ||
		mchunk_free((t_memchunk *)((size_t)a - sizeof(t_memchunk))) != 0)
	{
		ft_putfmt("Can't unmap allocator, memory\
criticaly corupted have to exit");
	}
}
