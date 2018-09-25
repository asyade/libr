#include "libr.h"

t_bheap *bheap_new(void *buffer, size_t buffer_size, size_t elem_size, t_bheap_cmpf cmpf)
{
    t_bheap *heap;

    heap = (t_bheap *)buffer;
    heap->capacity = (buffer_size - sizeof(t_bheap)) / elem_size;
    heap->elem_size = elem_size;
    heap->size = 0;
    heap->buffer_size = buffer_size;
    heap->cmpf = cmpf;
    return (heap);
}

size_t bheap_insert(t_bheap *heap, void *elem)
{
    if (heap->capacity <= heap->size)
        return (-1);
    ft_memcpy((unsigned char *)BH_INDEX(heap, heap->size++), elem, heap->elem_size);
    return (0);
}

int bheap_remove(t_bheap *heap, size_t index);
void *bh_index(t_bheap *ptr, size_t index);
