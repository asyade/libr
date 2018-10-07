/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   binary_heap.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acorbeau <acorbeau@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/10/01 16:16:50 by acorbeau          #+#    #+#             */
/*   Updated: 2018/10/07 18:28:13 by acorbeau         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libr.h"

t_bheap		*bheap_new(void *bf, size_t bs, size_t es, BHCMPF cmpf)
{
	t_bheap	*heap;

	if (bf == NULL || bf == MMAP_NULL)
		return (NULL);
	heap = (t_bheap *)bf;
	heap->capacity = (bs - sizeof(t_bheap)) / es;
	heap->elem_size = es;
	heap->size = 0;
	heap->buffer_size = bs;
	heap->cmpf = cmpf;
	return (heap);
}

size_t		bheap_insert(t_bheap *h, void *elem)
{
	if (h->capacity - 1 <= h->size)
		return (BH_NOTFOUND);
	ft_memcpy((unsigned char *)BH_INDEX(h, h->size), elem, h->elem_size);
	return (bheap_travers_up(h, h->size++));
}

int			bheap_remove(t_bheap *h, size_t i)
{
	if (i >= h->size)
		return (-1);
	if (i == --h->size)
		return (0);
	ft_memcpy(BH_INDEX(h, i), BH_INDEX(h, h->size), h->elem_size);
	bheap_heapify(h, i);
	return (0);
}

size_t		bheap_find(t_bheap *heap, void *value, size_t index)
{
	int		diff;
	size_t	ret;

	if ((diff = heap->cmpf(BH_INDEX(heap, index), value)) == 0)
		return (index);
	if (diff > 0)
		return (BH_NOTFOUND);
	if (BH_RIGHT(index) < heap->size &&
		(ret = bheap_find(heap, value, BH_RIGHT(index))) != BH_NOTFOUND)
		return (ret);
	if (BH_LEFT(index) < heap->size &&
		(ret = bheap_find(heap, value, BH_LEFT(index))) != BH_NOTFOUND)
		return (ret);
	return (BH_NOTFOUND);
}
