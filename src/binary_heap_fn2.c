/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   binary_heap_fn2.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acorbeau <acorbeau@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/10/07 18:27:53 by acorbeau          #+#    #+#             */
/*   Updated: 2018/10/07 18:28:09 by acorbeau         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libr.h"

size_t		bheap_travers_up(t_bheap *heap, size_t index)
{
	void	*self;
	void	*parent;

	while (index > 0)
	{
		self = BH_INDEX(heap, index);
		parent = BH_INDEX(heap, BH_PARENT(index));
		if ((heap->cmpf)(self, parent) < 0)
			ft_memswap(self, parent, heap->elem_size);
		else
			break ;
		index = BH_PARENT(index);
	}
	return (index);
}

size_t		bheap_travers_down(t_bheap *h, size_t index)
{
	void	*left;
	void	*right;
	void	*self;
	void	*smalset;

	self = BH_INDEX(h, index);
	left = BH_INDEX(h, BH_LEFT(index));
	right = BH_INDEX(h, BH_RIGHT(index));
	if (BH_LEFT(index) < h->size && BH_RIGHT(index) < h->size)
		smalset = ((h->cmpf)(left, right) < 0) ? left : right;
	else if (BH_LEFT(index) < h->size)
		smalset = left;
	else
		return (index);
	if ((h->cmpf)(self, smalset) > 0)
	{
		ft_memswap(self, smalset, h->elem_size);
		return (bheap_travers_down(h, MC1(h, smalset) / h->elem_size));
	}
	return (index);
}

size_t		bheap_heapify(t_bheap *h, size_t i)
{
	if (i > 0 && h->cmpf(BH_INDEX(h, i), BH_INDEX(h, BH_PARENT(i))) < 0)
		return (bheap_travers_up(h, i));
	else
		return (bheap_travers_down(h, i));
}
