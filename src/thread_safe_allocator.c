/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   thread_safe_allocator.c                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acorbeau <acorbeau@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/10/07 18:11:02 by acorbeau          #+#    #+#             */
/*   Updated: 2018/10/07 18:29:12 by acorbeau         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libr.h"

int				arrange_allocator_heap(t_memalloc *a, size_t buffer_size)
{
	size_t		med;
	t_bheap		*new;

	med = buffer_size / 2;
	new = (t_bheap *)((size_t)AEE(a) + AEE(a)->buffer_size + sizeof(t_bheap));
	ft_memmove(new, AUE(a), AUE(a)->buffer_size);
	AEE(a)->buffer_size = med - sizeof(t_bheap);
	AUE(a)->buffer_size = med - sizeof(t_bheap);
	AEE(a)->capacity = AEE(a)->buffer_size / sizeof(t_mementry);
	AUE(a)->capacity = AUE(a)->buffer_size / sizeof(t_mementry);
	return (0);
}

int				try_expande_allocator_heap(t_memalloc *a)
{
	t_memchunk	*new;
	t_bheap		*empty;
	t_bheap		*used;
	size_t		replace;

	replace = AEE(a)->buffer_size + AUE(a)->buffer_size + (2 * sizeof(t_bheap));
	replace *= 2;
	if ((new = mchunk_alloc(replace)) == NULL)
		return (1);
	replace = new->size;
	empty = (t_bheap *)(new + 1);
	used = (t_bheap *)((size_t)empty + (replace / 2));
	ft_memcpy(empty, AEE(a), AEE(a)->buffer_size + sizeof(t_bheap));
	ft_memcpy(used, AUE(a), AUE(a)->buffer_size + sizeof(t_bheap));
	mchunk_free((t_memchunk *)AEE(a) - 1);
	used->buffer_size = (replace / 2) - sizeof(t_bheap);
	empty->buffer_size = (replace / 2) - sizeof(t_bheap);
	used->capacity = used->buffer_size / sizeof(t_mementry);
	empty->capacity = empty->buffer_size / sizeof(t_mementry);
	AEE(a) = empty;
	AUE(a) = used;
	return (0);
}

void			*safe_memalloc_alloc(t_memalloc *a, size_t size, int retry)
{
	void		*ptr;
	int			err;

	ptr = NULL;
	if (a->empty_entries->capacity < a->empty_entries->size + 3 ||
		a->used_entries->capacity < a->used_entries->size + 2 ||
		(ptr = memalloc_alloc(a, size)) == NULL)
	{
		err = memalloc_geterr();
		if (err == E_INS_EMPTY || err == E_INS_USED)
		{
			memalloc_seterr(0);
			if (!retry)
				return (NULL);
			if (try_expande_allocator_heap(a) >= 0)
				return (safe_memalloc_alloc(a, size, 0));
			else
				memalloc_panic(E_EXPAND_HEAP);
		}
		else if (err != 0)
			memalloc_panic(err);
	}
	return (ptr);
}

int				safe_memalloc_free(t_memalloc *allocator, void *ptr)
{
	int			result;

	if ((result = memalloc_free(allocator, ptr)) < 0)
	{
		if (result == E_FIND_HEAP)
			return (1);
		memalloc_panic(result);
	}
	return (result);
}

int				safe_memalloc_expande(t_memalloc *a, void *ptr, size_t ns)
{
	int			result;

	if ((result = memalloc_try_expande(a, ptr, ns)) < 0)
		memalloc_panic(result);
	return (result);
}
