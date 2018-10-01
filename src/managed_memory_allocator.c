#include "libr.h"

#define ALLOCATORS_HEAP_SIZE 4096 //Need some test this is a random value

int memallocator_cmpf(void *aa, void *bb)
{
    size_t a;
    size_t b;

    a = (size_t)aa;
    b = (size_t)bb;
    if (a == b)
        return (0);
    return (a < b) ? -1 : 1;
}

t_bheap *mmemalloc_heap()
{
    static t_bheap *heap = NULL;
    t_memchunk *chunk;

    if (heap == NULL)
    {
        if ((chunk = mchunk_alloc(ALLOCATORS_HEAP_SIZE)) == NULL)
            memalloc_panic(E_NOMEM);
        heap = bheap_new(chunk + 1, chunk->size, sizeof(t_memalloc), memallocator_cmpf);
    }
    return (heap);
}

void *find_and_alloc(size_t size, size_t index)
{
    t_bheap *heap;
    t_memalloc *node;
    void *ptr;

    heap = mmemalloc_heap();
    if (index >= heap->size)
        return (NULL);
    node = (t_memalloc *)(heap + 1) + index;
    if (node->range.min < size && node->range.max > size &&
        (ptr = safe_memalloc_alloc(node, size, 1)) != NULL)
        return (ptr);
    if ((ptr = find_and_alloc(size, BH_LEFT(index))) != NULL)
        return (ptr);
    if ((ptr = find_and_alloc(size, BH_RIGHT(index))) != NULL)
        return (ptr);
    return (NULL);
}

t_memalloc *memalloc_new_range(size_t range)
{
    if (range < 128)
        return (memalloc_new(1024 * 1024, 4096, (t_szrange){0, 128}));
    if (range < 1024)
        return (memalloc_new(1024 * 1024 * 8, 4096, (t_szrange){0, 1024}));
    return (memalloc_new(range, 1024, (t_szrange){-1, -1}));
}

void *insert_and_alloc(size_t range)
{
    t_memalloc *allocator;

    if ((allocator = memalloc_new_range(range)) == NULL)
        return (NULL);
    if (bheap_insert(mmemalloc_heap(), allocator) == BH_NOTFOUND)
        memalloc_panic(E_NOMEM); //todo better error handling
    return (memalloc_alloc(allocator, range));
}

void *mmemalloc_alloc(size_t size)
{
    void *ptr;

    if ((ptr = find_and_alloc(size, 0)) != NULL)
        return (ptr);
    return (insert_and_alloc(size));
}

t_memalloc *find_allocator_by_addr(void *ptr, size_t index)
{
    t_bheap *heap;
    t_memalloc *allocator;

    heap = mmemalloc_heap();
    if (index >= heap->size)
        return (NULL);
    allocator = (t_memalloc *)(heap + 1) + index;
    if ((size_t)allocator < (size_t)ptr && (size_t)ptr < (size_t)(allocator + 1) + allocator->buffer_size)
        return (allocator);
    return (NULL);
}

void mmemalloc_free(void *ptr)
{
    t_memalloc *allocator;

    if ((allocator = find_allocator_by_addr(ptr, 0)) == NULL)
        memalloc_panic(E_OVERFLOW); //TODO better error handling
    if (memalloc_free(allocator, ptr) < 0)
        memalloc_panic(E_OVERFLOW);
}

//int mmemalloc_expande(void *ptr, size_t new_size)
//{
//}