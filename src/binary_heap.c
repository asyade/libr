#include "libr.h"

t_bheap *bheap_new(void *buffer, size_t buffer_size, size_t elem_size, t_bheap_cmpf cmpf)
{
	t_bheap *heap;

	if (buffer == NULL || buffer == MMAP_NULL)
		return (NULL);
	heap = (t_bheap *)buffer;
	heap->capacity = (buffer_size - sizeof(t_bheap)) / elem_size;
	heap->elem_size = elem_size;
	heap->size = 0;
	heap->buffer_size = buffer_size;
	heap->cmpf = cmpf;
	return (heap);
}

size_t bheap_travers_up(t_bheap *heap, size_t index)
{
	void *self;
	void *parent;

	while (index > 0)
	{
		self = BH_INDEX(heap, index);
		parent = BH_INDEX(heap, BH_PARENT(index));
		if ((heap->cmpf)(self, parent) < 0)
			ft_memswap(self, parent, heap->elem_size);
		else
			break;
		index = BH_PARENT(index);
	}
	return (index);
}

size_t bheap_travers_down(t_bheap *heap, size_t index)
{
	void *left;
	void *right;
	void *self;
	void *smalset;

	self = BH_INDEX(heap, index);
	left = BH_INDEX(heap, BH_LEFT(index));
	right = BH_INDEX(heap, BH_RIGHT(index));
	if (BH_LEFT(index) < heap->size && BH_RIGHT(index) < heap->size)
		smalset = ((heap->cmpf)(left, right) <= 0) ? left : right;
	else if (BH_LEFT(index) < heap->size)
		smalset = left;
	else
		return (index);
	if ((heap->cmpf)(self, smalset) > 0)
	{
		ft_memswap(self, smalset, heap->elem_size);
		return (bheap_travers_down(heap, ((size_t)smalset - (size_t)(heap + 1)) / heap->elem_size));
	}
	return (index);
}

size_t bheap_heapify(t_bheap *heap, size_t index)
{
	if (index > 0 && heap->cmpf(BH_INDEX(heap, index), BH_INDEX(heap, BH_PARENT(index))) < 0)
		return (bheap_travers_up(heap, index));
	else
		return (bheap_travers_down(heap, index));
}

size_t bheap_insert(t_bheap *heap, void *elem)
{
	if (heap->capacity <= heap->size)
		return (-1);
	ft_memcpy((unsigned char *)BH_INDEX(heap, heap->size), elem, heap->elem_size);
	return (bheap_travers_up(heap, heap->size++));
}

int bheap_remove(t_bheap *heap, size_t index)
{
	if (index >= heap->size)
		return (0);
	if (index == --heap->size)
		return (0);
	ft_memcpy(BH_INDEX(heap, index), BH_INDEX(heap, heap->size), heap->elem_size);
	bheap_heapify(heap, index);
	return (0);
}

size_t bheap_find(t_bheap *heap, void *value, size_t index)
{
	int diff;
	size_t ret;

	if ((diff = heap->cmpf(BH_INDEX(heap, index), value)) == 0)
		return (index);
	if (diff > 0)
		return (BH_NOTFOUND);
	if (BH_LEFT(index) < heap->size &&
		(ret = bheap_find(heap, value, BH_LEFT(index))) != BH_NOTFOUND)
		return (ret);
	if (BH_RIGHT(index) < heap->size &&
		(ret = bheap_find(heap, value, BH_RIGHT(index))) != BH_NOTFOUND)
		return (ret);
	return (BH_NOTFOUND);
}
